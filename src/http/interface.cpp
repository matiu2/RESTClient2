#include "interface.hpp"

#include "../tcpip/interface.hpp"

namespace RESTClient {
namespace http {

void spawn(std::function<void(boost::asio::yield_context)> worker) {
  auto io = tcpip::getService();
  spawn(*io, worker);
}

void run() {
  auto io = tcpip::getService();
  io->run();
}

} /* http  */
} /* RESTClient  */ 
