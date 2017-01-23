#pragma once

#include <string>
#include <map>

namespace RESTClient {
namespace http {
    
struct URL {
  std::string protocol;
  std::string username;
  std::string password;
  std::string hostname;
  unsigned short port;
  std::string path;
  std::map<std::string, std::string> params;
  URL(std::string url);
  URL() {}
  std::string whole() const;
  /// Returns just the host part: eg. http://some-domain.com:2020
  /// Useful for the tcp connection
  std::string host_part() const;
  /// Returns everything after the host part
  std::string path_part() const;
  URL& operator =(const std::string& url);
  bool is_ssl() const { return protocol == "https"; }
};
  
} /* http */ 
} /* RESTClient */ 

