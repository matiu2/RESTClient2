#include "interface.hpp"

#include <boost/asio.hpp>

namespace tcpip {

using namespace boost::asio;

thread_local std::shared_ptr<io_service> gService;

io_service &getService() {
  if (not gService) {
    gService.reset(new io_service());
  }
  return *(gService.get());
}

} // tcpip 
