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
  std::ostream* body1 = nullptr;
  const std::string* body2 = nullptr;
  std::istream* _outBody = nullptr;
  std::string url;
  std::string _verb = "GET";
  yield_context yield;

public:
  Request( std::string url, yield_context yield) : url(url), yield(yield) {}
  Request &headers(const Headers &newHeaders) {
    _headers = &newHeaders;
    return *this;
  }
  Request& add_header(std::string  key, std::string value) {
    extra_headers.insert(std::make_pair(key, value));
    return *this;
  }
  /// You need to keep newBody alive longer than the request, as we retain a pointer to it
  Request& body(const std::string& newBody) {
    body2 = &newBody;
    return *this;
  }
  /// You need to keep newBody alive longer than the request, as we retain a pointer to it
  Request& body(std::ostream& newBody) {
    body1 = &newBody;
    return *this;
  }
  Request& verb(std::string verb) {
    _verb = std::move(verb);
    return *this;
  }
  /// Causes the eventual body to be saved to a stream (rather than just as a
  /// string in the Response object). The resulting Response::body string will be empty
  Request& saveToStream(std::istream& responseBody) {
    _outBody = &responseBody;
    return *this;
  }
  Response go() const;
};

} /* http */ 
} /* RESTClient */ 
