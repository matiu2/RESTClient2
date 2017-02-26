#include "Connection.hpp"
#include "interface.hpp"
#include "Exceptions.hpp"

#include <boost/asio/basic_streambuf.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

#include <string>
#include <iterator>
#include <iostream>

namespace asio = boost::asio;
namespace ssl = asio::ssl;

namespace RESTClient {
namespace tcpip {

struct SSL {
  ssl::context ctx;
  ssl::stream<asio::ip::tcp::socket&> s;
  SSL(tcp::socket &sock) : ctx(ssl::context::tlsv12), s(sock, ctx) {}
};

class Connection::impl {
public:
  std::shared_ptr<io_service> io;
  std::string address;
  std::string service;
  tcp::socket socket;
  yield_context yield;
  std::unique_ptr<SSL> ssl;
  asio::streambuf _buf; // Incoming data buffer buffer

  impl(std::string address, std::string service, yield_context yield,
       bool is_ssl)
      : io(getService()), address(address), service(service), socket(*io),
        yield(yield), ssl(), _buf() {
    // Lookup the IP addresse
    asio::ip::basic_resolver_iterator<boost::asio::ip::tcp> endpoints;
    try {
      endpoints = lookup(address, service, yield);
    } catch (const std::exception &e) {
      throw DNSLookupError(address, e.what());
    } catch (...) {
      throw DNSLookupError(address, "Unkown");
    }
    // Connect the socket
    try {
      asio::async_connect(socket, endpoints, yield);
    } catch (const std::exception &e) {
      throw ConnectionError(address, service, e.what());
    } catch (...) {
      throw ConnectionError(address, service, "Unkown");
    }
    // Perform ssl startup if requested
    if (is_ssl) {
      ssl.reset(new SSL(socket));
      ssl->ctx.set_default_verify_paths();
      socket.set_option(tcp::no_delay(true));
      // Perform SSL handshake and verify the remote host's
      // certificate.
      ssl->s.set_verify_mode(ssl::verify_peer);
      ssl->s.set_verify_callback(ssl::rfc2818_verification(address));
      ssl->s.handshake(decltype(ssl->s)::client);
    }
  }

  ~impl() {
    if (ssl) {
      boost::system::error_code ec;
      ssl->s.async_shutdown(yield[ec]);
      using asio::error::misc_errors;
      using asio::error::basic_errors;
      const auto &misc_cat = asio::error::get_misc_category();
      const auto &ssl_cat = asio::error::get_ssl_category();
      // This error means the remote party has initiated has already closed the
      // underlying transport (TCP FIN) without shutting down the SSL.
      // It may be a truncate attack attempt, but nothing we can do about it
      // except close the connection.
      if (ec.category() == ssl_cat &&
          ec.value() == ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SHORT_READ)) {
        // SSL Shutdown - remote party just dropped TCP FIN instead of closing
        // SSL protocol. Possible truncate attack - closing connection.
        return;
      }
      // We are the first one to run ssl_shutdown, and remote party responded in
      // kind, just continue
      if (ec.category() == misc_cat && ec.value() == misc_errors::eof) {
        return;
      }
      // The remote party sent ssl_shutdown, then just dropped the connection
      if (ec.category() == misc_cat &&
          ec.value() == basic_errors::operation_aborted) {
        return;
      }
      // Everything went as planned
      if (ec.category() == boost::system::system_category() &&
          ec.value() == boost::system::errc::success) {
        return;
      }
      // Something scary happened, log an error (throw an exception too)
      throw SSLShutdownError(ec);
    }
    if (socket.is_open()) {
      socket.close();
    }
  }

  void send(const std::string &data) {
    if (ssl)
      asio::async_write(ssl->s, asio::buffer(data), yield);
    else
      asio::async_write(socket, asio::buffer(data), yield);
  }

  void recv(std::string &data, size_t size) { 
    SpyGuard fromNet = spy(size);
    assert(fromNet.size() == size);
    data.reserve(data.size() + size);
    std::copy(fromNet.begin(), fromNet.begin() + size,
              std::back_inserter(data));
  }

  void recv(std::string &out, char delim) {
    // Copy whatever we have in the buffer to out
    SpyGuard buf = spy('\n');
    auto found = std::find(buf.begin(), buf.end(), delim) + 1;
    auto sz = std::distance(buf.begin(), found);
    out.reserve(out.size() + sz);
    std::copy(buf.begin(), found, std::back_inserter(out));
  }

  /// Recieve 'n' bytes into a stream
  void recv(std::ostream &data, size_t size) {
    SpyGuard buf = spy(size);
    std::copy(buf.begin(), buf.end(), std::ostream_iterator<char>(data));
  }

  SpyGuard spyGuard(SpyRange::iterator begin, SpyRange::iterator end) {
    using namespace std::placeholders;
    return SpyGuard([this](size_t size) { this->consume(size); },
                    SpyRange(begin, end));
  }

  SpyGuard spy(size_t size) {
    size_t startingSize = _buf.size();
    size_t toGet = (startingSize < size) ? size - startingSize : 0;
    if (ssl)
      asio::async_read(ssl->s, _buf, asio::transfer_at_least(toGet), yield);
    else
      asio::async_read(socket, _buf, asio::transfer_at_least(toGet), yield);
    assert(_buf.size() >= size);
    const auto &buffers = _buf.data();
    auto begin = asio::buffers_begin(buffers);
    return spyGuard(begin, begin+size);
  }

  SpyGuard spy(char delim) {
    if (_buf.size() == 0) {
      if (ssl) {
        asio::async_read_until(ssl->s, _buf, delim, yield);
      } else {
        asio::async_read_until(socket, _buf, delim, yield);
      }
    }
    while (true) {
      // First check if we already have it in the buffer from previous reads
      // before trying to read it
      // TODO: maybe we don't need to get the start every time; just the end
      auto in = asio::buffers_begin(_buf.data());
      auto eos = asio::buffers_end(_buf.data());
      auto found = std::find(in, eos, delim);
      if (found != eos)
        return spyGuard(in, found + 1);
      // The current buffer doesn't contain 'delim'.
      // We need to read more data from the net.
      // TODO: Maybe we need to specify a timeout and catch timeout errors here ..
      if (ssl) {
        asio::async_read_until(ssl->s, _buf, delim, yield);
      } else {
        asio::async_read_until(socket, _buf, delim, yield);
      }
    }
  }

  SpyRange spyAvailable() {
    if (_buf.size() == 0) {
      if (ssl) {
        asio::async_read(ssl->s, _buf, boost::asio::transfer_at_least(1), yield);
      } else {
        asio::async_read(socket, _buf, boost::asio::transfer_at_least(1), yield);
      }
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
void Connection::recv(std::ostream &data, size_t size) { m->recv(data, size); }
SpyGuard Connection::spy(char delim) { return m->spy(delim); }
SpyGuard Connection::spy(size_t size) { return m->spy(size); }
void Connection::consume(size_t size) { m->consume(size); }
yield_context& Connection::yield() {return m->yield; }

} // tcpip
} /* RESTClient */ 
