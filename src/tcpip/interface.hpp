#pragma once
/// Interface to the tcpip section of the library

#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Connection.hpp"

namespace RESTClient {
namespace tcpip {

using boost::asio::yield_context;
using boost::asio::io_service;
using boost::asio::spawn;
using namespace boost::asio::ip;

std::shared_ptr<io_service> getService();

tcp::resolver::iterator lookup(std::string address, std::string service,
                               yield_context yield);

Connection connect(std::string address, std::string service,
                   yield_context yield, bool is_ssl = false);

} // tcpip 
} /* RESTClient */ 
