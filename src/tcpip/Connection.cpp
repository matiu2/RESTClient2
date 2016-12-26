#include "Connection.hpp"
#include "interface.hpp"

#include <boost/asio/basic_streambuf.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

#include <string>
#include <iterator>
#include <iostream>

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

  yield_context yield;
  bool is_ssl;

  impl(std::string address, std::string service, yield_context yield,
       bool is_ssl)
      : io(getService()), address(address), service(service), socket(*io),
        _buf(), yield(yield), is_ssl(is_ssl) {
    // Connect
    auto endpoints = lookup(address, service, yield);
    asio::async_connect(socket, endpoints, yield);
  }

  ~impl() {
  }

  void send(std::string data) {
    asio::async_write(socket, asio::buffer(data), yield);
  }

  void recv(std::string &data, size_t size) { 
    asio::async_read(socket, _buf, asio::transfer_exactly(size), yield);
    assert(_buf.size() >= size);
    data.reserve(data.size() + _buf.size());
    auto begin = asio::buffers_begin(_buf.data());
    std::copy(begin, begin + size, std::back_inserter(data));
    _buf.consume(size);
  }

  void recv(std::string &out, char delim) {
    // Copy whatever we have in the buffer to out
    while (true) {
      auto in = asio::buffers_begin(_buf.data());
      auto eos = asio::buffers_end(_buf.data());
      auto found = std::find(in, eos, delim);
      auto sz = std::distance(in, found);
      out.reserve(out.size() + sz);
      std::copy(in, found, std::back_inserter(out));
      _buf.consume(sz);
      if (found != eos)
        break;
      // We have copied all the cached buffer, but still not come across delim,
      // We need to read more data from the net
      // TODO: Maybe we need to specify a timeout and catch timeout errors here ..
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
