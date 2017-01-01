#pragma once

#include "Headers.hpp"

namespace RESTClient {
namespace http {

struct Response {
  Headers headers;
  char code[4]; /// HTTP respnonse code (eg. 200 404 513 etc)
  /// This may be empty if you called Request::body(std::ostream& newBody);
  std::string body;
};
  
} /* http */ 
} /* RESTClient */ 
