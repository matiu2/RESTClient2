#pragma once

#include <string>
#include <map>

namespace RESTClient {
namespace http {
    
struct URL {
  std::string protocol;
  std::string hostname;
  unsigned short port;
  std::string path;
  std::map<std::string, std::string> params;
  URL(std::string url);
  std::string original() const;
  URL& operator =(const std::string& url);
};
  
} /* http */ 
} /* RESTClient */ 

