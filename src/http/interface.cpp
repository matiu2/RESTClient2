#include "interface.hpp"

#include "../tcpip/interface.hpp"

namespace RESTClient {
namespace http {

void spawn(std::function<void(boost::asio::yield_context)>&& worker) {
  auto io = tcpip::getService();
  tcpip::spawn(*io, std::move(worker));
}

size_t run() {
  auto io = tcpip::getService();
  return io->run();
}

} /* http  */
} /* RESTClient  */ 
