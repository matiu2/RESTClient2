#include "ReadResponse.hpp"

#include "../tcpip/interface.hpp"
#include "BodyDecoders.hpp"

#include <string>
#include <sstream>
#include <iostream>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/vector_tie.hpp>
#include <boost/spirit/home/x3.hpp>

#include <iostream>

using namespace std::string_literals;

namespace header_parser {

using namespace boost::spirit::x3;

auto firstLine = lit("HTTP/1.1") >> ushort_ >> +(char_ - '\r') >> "\r\n";
auto header = +(char_ - ':') >> ':' >> +(char_ - '\r') >> "\r\n";
auto spacer = (lit(' ') | lit('\t'));

} /* header_parser */ 

namespace RESTClient {
namespace http {

bool is_chunked(const Headers &headers) {
  auto found = headers.find("Transfer-Encoding");
  if (found == headers.end())
    return found->second == "chunked";
  else
    return false;
}

size_t getContentLength(const Headers& headers) {
  auto found = headers.find("Content-Length");
  if (found == headers.end())
    return std::stoul(found->second);
  // If there is no contentLength, the decoder shouldn't be calling this
  std::stringstream msg;
  msg << "There is no contentLength in these headers: \n";
  for (const auto& pair : headers) {
    msg << pair.first << ": " << pair.second << '\n';
  }
  throw std::runtime_error(msg.str());
}

/// Allows us to extend a string with a boost::iterator_range
template <typename T>
std::string& operator +=(std::string& a, boost::iterator_range<T> b) {
    std::string temp;
    std::copy(b.begin(), b.end(), std::back_inserter(a));
    return a;
}

/// Allows us to extend a string with a boost::iterator_range
template <typename T>
std::string operator +(std::string a, boost::iterator_range<T> b) {
    a += b;
    return a;
}

void readHeadersPart(tcpip::Connection &conn, Response &out) {
  // Now we read the reply .. first the headers

  // Read the first line
  {
    auto firstLineOut = boost::fusion::vector_tie(out.code, out.ok);
    auto line = conn.spy('\n');
    bool ok = boost::spirit::x3::phrase_parse(
        line.begin(), line.end(), header_parser::firstLine,
        header_parser::spacer, firstLineOut);
    if (!ok) {
      throw std::runtime_error("Unable to parse first line: '"s + line + "'");
    }
  }

  // Read the first line
  while (true) {
    std::string key, val;
    auto parsed = boost::fusion::vector_tie(key, val);
    auto line = conn.spy('\n');
    if (line == "\r\n")
      break;
    using namespace std;
    std::string temp;
    std::copy(line.begin(), line.end(), std::back_inserter(temp));
    cout << temp;
    bool ok = boost::spirit::x3::phrase_parse(line.begin(), line.end(),
                                              header_parser::header,
                                              header_parser::spacer, parsed);
    if (!ok) {
      throw std::runtime_error("Unable to parse header: '"s + line + "'");
    }
    out.headers.emplace(std::move(key), std::move(val));
  }
  // Now check what sort of body encoding we have
  auto found = out.headers.find("Transfer-Encoding");
  std::string encoding("identity");
  if (found != out.headers.end())
    encoding = found->second;
}

void readResponse(tcpip::Connection &conn, Response &out) {
  readHeadersPart(conn, out);
  // Now read the body
  // We are saving to the string in the response
  // If we're reading the reply to a stream..
  if (is_chunked(out.headers)) {
    auto readLine = [&conn](std::string &line) { conn.recv(line); };
    auto readN = [&conn](size_t n, std::string &line) { conn.recv(line, n); };
    decoder::chunked(readLine, readN, out.body);
  } else {
    // Just recv the Content-Length to the body
    size_t len = getContentLength(out.headers);
    conn.recv(out.body, len);
  }
}

void readResponse(tcpip::Connection &conn, Response &out, std::ostream &body) {
  readHeadersPart(conn, out);
  // Now read the body
  // If we're reading the reply to a stream..
  if (is_chunked(out.headers)) {
    auto readLine = [&conn](std::string &line) { conn.recv(line); };
    auto readN = [&conn](size_t n, std::ostream &line) { conn.recv(line, n); };
    decoder::chunked(readLine, readN, body);
  } else {
    conn.recv(body, getContentLength(out.headers));
  }
}

} /* http */
} /* RESTClient */
