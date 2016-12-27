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

  void send(const std::string &data) {
    asio::async_write(socket, asio::buffer(data), yield);
  }

  void recv(std::string &data, size_t size) { 
    SpyRange fromNet = spy(size);
    assert(fromNet.size() > size);
    data.reserve(data.size() + size);
    std::copy(fromNet.begin(), fromNet.begin() + size,
              std::back_inserter(data));
    consume(size);
  }

  void recv(std::string &out, char delim) {
    // Copy whatever we have in the buffer to out
    auto range = spy('\n');
    auto found = std::find(range.begin(), range.end(), delim);
    auto sz = std::distance(range.begin(), found);
    out.reserve(out.size() + sz);
    std::copy(range.begin(), found, std::back_inserter(out));
    consume(sz + 1);
  }

  Connection::SpyRange spy(size_t size) {
    size_t startingSize = _buf.size();
    size_t toGet = (startingSize < size) ? size - startingSize : 0;
    asio::async_read(socket, _buf, asio::transfer_at_least(toGet), yield);
    assert(_buf.size() >= size);
    const auto &buffers = _buf.data();
    return SpyRange(asio::buffers_begin(buffers), asio::buffers_end(buffers));
  }

  Connection::SpyRange spy(char delim) {
    if (_buf.size() == 0)
      asio::async_read_until(socket, _buf, delim, yield);
    while (true) {
      // First check if we already have it in the buffer from previous reads
      // before trying to read it
      auto in = asio::buffers_begin(_buf.data());
      auto eos = asio::buffers_end(_buf.data());
      auto found = std::find(in, eos, delim);
      if (found != eos)
        break;
      // The current buffer doesn't contain 'delim'.
      // We need to read more data from the net.
      // TODO: Maybe we need to specify a timeout and catch timeout errors here ..
      asio::async_read_until(socket, _buf, delim, yield);
    }
    const auto &buffers = _buf.data();
    return SpyRange(asio::buffers_begin(buffers), asio::buffers_end(buffers));
  }

  void consume(size_t size) {
    _buf.consume(size);
  }
};

Connection::Connection(std::string address, std::string service,
                       yield_context yield, bool is_ssl)
    : m(address, service, yield, is_ssl) {}

Connection::~Connection() {}

void Connection::send(const std::string &data) { m->send(std::move(data)); }

void Connection::recv(std::string &data, size_t size) { m->recv(data, size); }
void Connection::recv(std::string &data, char delim) { m->recv(data, delim); }
Connection::SpyRange Connection::spy(char delim) { return m->spy(delim); }
Connection::SpyRange Connection::spy(size_t size) { return m->spy(size); }
void Connection::consume(size_t size) { m->consume(size); }

} // tcpip
} /* RESTClient */ 
