#include "Request.hpp"

#include "url.hpp"
#include "sendBody.hpp"
#include "ReadResponse.hpp"
#include "../logging.hpp"
#include "istream_for_beast.hpp"

#include <sstream>
#include <boost/optional.hpp>
#include <boost/beast.hpp>

namespace RESTClient {
namespace http {

/// Transmits the headers; if an item is duplicated in both sets; extra_headers takes presedence.
void transmitHeaders(const Headers *defaultHeaders,
                     const Headers &extra_headers, tcpip::Connection &conn) {
  LOG_SCOPE_FUNCTION(5);
  DLOG_S(8) << "Sending headers for Request";
  auto sendHeader = [&](const auto &header) {
      DLOG_S(8) << header.first << ": " << header.second;
      conn.send(header.first);
      conn.send(": ");
      conn.send(header.second);
      conn.send("\r\n");
  };
  if (defaultHeaders) {
    DLOG_S(8) << "Sending default headers";
    for (const auto& header: *defaultHeaders) {
      auto found = extra_headers.find(header.first);
      // Only send headers that aren't in extra_headers already
      if (found == extra_headers.end()) {
        DLOG_S(8) << "Sending default header: " << header.first << ": "
                  << header.second;
        sendHeader(header);
      }
    }
  }
  for (const auto &header : extra_headers) {
        DLOG_S(8) << "Sending extra header: " << header.first << ": "
                  << header.second;
    sendHeader(header);
  }
}

/// Returns the value of a header if it exists
boost::optional<std::string> checkForHeader(const std::string& headerName, const Headers *defaultHeaders,
                                            const Headers &extra_headers) {
  boost::optional<std::string> result;
  // First look in the default headers
  if (defaultHeaders) {
    auto found = defaultHeaders->find(headerName);
    if (found != defaultHeaders->end())
      result = found->second;
  }
  // Now look in the extra headers
  auto found = extra_headers.find(headerName);
  if (found != extra_headers.end())
    result = found->second;
  return result;
}

boost::optional<size_t> checkForContentLength(const Headers *defaultHeaders,
                                              const Headers &extra_headers) {
  boost::optional<size_t> result;
  boost::optional<std::string> temp = checkForHeader("Content-Length", defaultHeaders, extra_headers);
  if (temp)
    result = std::stoul(temp.value());
  return result;
};

Response Request::go() const {
  // Get a TCP/IP connection from the connection pool
  URL url(this->url);
  
  using namespace boost::beast;
  using namespace boost::beast::http;

  // Generic lambda that does the work of filling the request object
  auto fill = [](auto &req) {
  };


  // First see what kind of body we need to send
  if (body_stream) {
    request<IStreamBody> req;
    fill(req);
  } else if (body_string) {
    request<StringBody> req;
    fill(req);
  } else {
    request<EmptyBody> req;
    fill(req);
  }
  


  // Send the request line
  LOG_S(8) << "Request First Line: " << _verb << " " << url.path_part()
           << " HTTP/1.1";
  conn.send(_verb + ' ');
  conn.send(url.path_part());
  conn.send(" HTTP/1.1\r\n");
  // Send the Host header
  LOG_S(8) << "   Host: " << url.hostname;
  conn.send("Host: ");
  conn.send(url.hostname);
  conn.send("\r\n");
  // Returns the content length if it was set in any headers
  // Send the headers
  transmitHeaders(_headers, extra_headers, conn);
  // See if we're sending a Content-Length header or transmitting in chunked
  // encoding
  boost::optional<size_t> contentLength(
      checkForContentLength(_headers, extra_headers));
  if ((!contentLength) && (!body_stream) && (body_string)) {
    // We have a body_string, so we know the content length; lets send it
    contentLength = body_string->size();
    conn.send("Content-Length: ");
    conn.send(std::to_string(contentLength.value()));
    conn.send("\r\n");
  } else if ((!contentLength) && (body_stream)) {
    if (body_stream) {
      // We don't know the Content-Length; we'll be transmitting in 'chunked' encoding
      boost::optional<std::string> te(checkForHeader("Transfer-Encoding", _headers, extra_headers));
      if (!te)
        conn.send("Transfer-Encoding: chunked\r\n");
    } else {
      // We have no body
      if (!contentLength) {
        // If we haven't sent the content length, send it now
        conn.send("Content-Length: 0\r\n\r\n");
      }
    }
  }
  // No more headers
  conn.send("\r\n");

  // Send the body
  if (body_stream) {
    // Send the stream contents up the Internet
    if (contentLength)
      sendStream(*body_stream, conn);
    else
      chunkedTransmit(*body_stream, conn);
  } else if (body_string) {
    // We have a string; just send it
    conn.send(*body_string);
  }

  // Now read the reply
  Response response;
  if (_outBody)
    readResponse(conn, response, *_outBody);
  else
    readResponse(conn, response);

  return response;
}

} /* http */
} /* RESTClient  */
