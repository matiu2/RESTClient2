#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace RESTClient {
namespace tcpip {

using namespace boost;
using namespace boost::asio::ip; // to access 'tcp::' namespace

struct Services {
  asio::io_service io_service;
  tcp::resolver resolver;
  Services();
  static std::shared_ptr<Services> instance();
};

  
} /* tcpip */
} /* RESTClient */ 
