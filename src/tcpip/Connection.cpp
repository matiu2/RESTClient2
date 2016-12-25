#include "Connection.hpp"
#include "Services.hpp"

#include <string>

namespace RESTClient {
namespace tcpip {

class Connection::impl {
public:
  std::shared_ptr<Services> services;
  std::string address;

  yield_context yield;
  bool is_ssl;

  impl(std::string address, yield_context yield, bool is_ssl)
      : services(Services::instance()), address(address), yield(yield),
        is_ssl(is_ssl) {
          // Connect
          tcp::resolver::query query(address, is_ssl ? "https" : "http");
          auto endpoint = services->resolver.async_resolve(query, yield);

        }

  void send(const std::string &data) {

  }

  void recv(std::string &data, size_t max) { 
  }

  void recv(std::string &data, char delim) {
  }
    
};

Connection::Connection(const std::string address, yield_context yield,
                       bool is_ssl)
    : m(address, yield, is_ssl) {}

void Connection::send(const std::string &data) { m->send(data); }

void Connection::recv(std::string &data, size_t max) { m->recv(data, max); }
void Connection::recv(std::string &data, char delim) { m->recv(data, delim); }

} // tcpip
} /* RESTClient */ 
