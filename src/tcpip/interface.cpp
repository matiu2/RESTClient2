#include "interface.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace RESTClient {
namespace tcpip {

using namespace boost::asio;

std::shared_ptr<io_service> gService;
thread_local std::unique_ptr<tcp::resolver> gResolver;

std::shared_ptr<io_service> getService() {
  if (not gService) {
    gService.reset(new io_service());
  }
  return gService;
}

tcp::resolver& resolver() {
  if (not gResolver) {
    gResolver.reset(new tcp::resolver(*getService()));
  }
  return *gResolver.get();
}

tcp::resolver::iterator lookup(std::string address, std::string service,
                               yield_context yield) {
  tcp::resolver::query q(address, service);
  return resolver().async_resolve(q, yield);
}

} // tcpip 
} /* RESTClient */ 
