#pragma once

#include "Headers.hpp"

namespace RESTClient {
namespace http {

struct Response {
  Headers headers;
  unsigned short code; /// HTTP respnonse code (eg. 200 404 513 etc)
  std::string ok; /// The word OK, or whatever else http returns on the first line
  /// This may be empty if you called Request::body(std::ostream& newBody);
  std::string body;
};
  
} /* http */ 
} /* RESTClient */ 
