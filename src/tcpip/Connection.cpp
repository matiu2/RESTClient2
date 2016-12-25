#include "Connection.hpp"
#include "interface.hpp"

#include <boost/asio/basic_streambuf.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>

#include <string>
#include <iterator>

namespace asio = boost::asio;

namespace RESTClient {
namespace tcpip {

class Connection::impl {
public:
  std::shared_ptr<io_service> io;
  std::string address;
  std::string service;
  tcp::socket socket;
  asio::streambuf _buf; // Incoming data buffer buffer
  std::istream buf;    // Incoming data buffer

  yield_context yield;
  bool is_ssl;

  impl(std::string address, std::string service, yield_context yield,
       bool is_ssl)
      : io(getService()), address(address), service(service), socket(*io),
        _buf(), buf(&_buf), yield(yield), is_ssl(is_ssl) {
    // Connect
    auto endpoints = lookup(address, service, yield);
    asio::async_connect(socket, endpoints, yield);
  }

  ~impl() {
  }

  void send(std::string data) {
    asio::async_write(socket, asio::buffer(std::move(data)), yield);
  }

  void recv(std::string &data, size_t size) { 
    asio::async_read(socket, _buf, asio::transfer_exactly(size), yield);
  }

  void recv(std::string &out, char delim) {
    // Copy whatever we have in the buffer to out
    while (true) {
      std::istream_iterator<char> in(buf);
      std::istream_iterator<char> eos;
      while (in != eos) {
        char c = *(in++);
        out.push_back(c);
        if (c == delim)
          return;
      }
      // We have copied all the old buffer, but still not come across delim, we
      // need to get more
      asio::async_read_until(socket, _buf, delim, yield);
    }
  }
};

Connection::Connection(std::string address, std::string service, yield_context yield,
                       bool is_ssl)
    : m(address, service, yield, is_ssl) {}

Connection::~Connection() {}

void Connection::send(std::string data) { m->send(std::move(data)); }

void Connection::recv(std::string &data, size_t size) { m->recv(data, size); }
void Connection::recv(std::string &data, char delim) { m->recv(data, delim); }

} // tcpip
} /* RESTClient */ 
