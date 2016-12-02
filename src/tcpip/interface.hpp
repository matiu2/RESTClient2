/// Interface to the tcpip section of the library

#include <boost/asio/io_service.hpp>

namespace tcpip {

using namespace boost::asio;

/// Get a copy of the thread-local global io_service
io_service& getService();
  
} // tcpip 
