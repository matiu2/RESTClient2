#pragma once

#include "Response.hpp"

#include <boost/asio/spawn.hpp>

namespace RESTClient {
namespace http {

using boost::asio::yield_context;

class Request {
private:
  const Headers* _headers = nullptr;
  Headers extra_headers;
  std::istream* body_stream = nullptr;
  const std::string* body_string = nullptr;
  std::istream* _outBody = nullptr;
  std::string url;
  std::string _verb = "GET";

public:
  Request( std::string url) : url(url) {}
  /// Set the default headers. Calls to add_header later will override anything
  /// in here with the same key
  Request &headers(const Headers &newHeaders) {
    _headers = &newHeaders;
    return *this;
  }
  /// Add a header to our 'extra_headers' field
  /// If you don't add 'Content-Length' or 'Transfer-Encoding: chunked' we'll do
  /// it for you at transmission time. Subsequent calls with the same key
  /// override previous calls
  Request& add_header(std::string  key, std::string value) {
    extra_headers.insert(std::make_pair(key, value));
    return *this;
  }
  /// Specify a string body
  /// You need to keep newBody alive longer than the request, as we retain a pointer to it
  Request& body(const std::string& newBody) {
    body_string = &newBody;
    return *this;
  }
  /// Specify a stream body
  /// If you don't specify a Content-Length header, it will be transmitted in chunked encoding
  /// You need to keep newBody alive longer than the request, as we retain a pointer to it
  Request& body(std::istream& newBody) {
    body_stream = &newBody;
    return *this;
  }
  Request& verb(std::string verb) {
    _verb = std::move(verb);
    return *this;
  }
  /// Causes the incoming (over tcpip) body to be saved to a stream (rather
  /// than just as a string in the Response object). The resulting
  /// Response::body string will be empty
  Request& saveToStream(std::istream& responseBody) {
    _outBody = &responseBody;
    return *this;
  }
  Response go(yield_context yield) const;
};

} /* http */ 
} /* RESTClient */ 
