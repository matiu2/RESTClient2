#pragma once

#include <string>
#include <map>

namespace RESTClient {

/// Splits a URL into parts (all we need right now)
struct URL {
  std::string protocol;
  std::string username;
  std::string password;
  std::string hostname;
  unsigned short port;
  std::string path;
  std::map<std::string, std::string> params;
  URL(const std::string& url);
  std::string url() const;
};

} /* RESTClient */ 
