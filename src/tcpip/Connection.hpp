#pragma once

#include "../pimpl.hpp"
#include "interface.hpp"
#include <boost/asio/spawn.hpp>

#include <string>

namespace RESTClient {
namespace tcpip {

using boost::asio::yield_context;

class Connection {
private:
  class impl;
  pimpl<impl> m;

public:
  Connection(tcp::resolver::iterator address, yield_context yield,
             bool is_ssl = true);
  void send(const std::string& data);
  void recv(std::string& data, std::size_t max);
  void recv(std::string& data, char delim);
};

} // tcpip 
} /* RESTClient */ 
