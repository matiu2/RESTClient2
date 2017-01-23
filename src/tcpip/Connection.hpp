#pragma once

#include "../pimpl.hpp"
#include "interface.hpp"
#include <boost/asio/spawn.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include <string>
#include <ostream>

namespace RESTClient {
namespace tcpip {

using boost::asio::yield_context;

struct SpyGuard;

class Connection {
public:
  using SpyIterator =
      boost::asio::buffers_iterator<boost::asio::const_buffers_1>;
  using SpyRange = boost::iterator_range<SpyIterator>;
private:
  class impl;
  pimpl<impl> m;

public:
  Connection(std::string address, std::string service, yield_context yield,
             bool is_ssl = false);
  ~Connection();
  /// Send data through the internet
  void send(const std::string& data);

  /// Receive exactly 'size' bytes and move it into a string
  void recv(std::string& data, size_t size);

  /// Receive until we hit some deliminator and move it into a string
  void recv(std::string& data, char delim);

  /// Receive a line and move it int a string
  void recv(std::string& data) { recv(data, '\n'); }

  /// Recieve 'n' bytes into a stream
  void recv(std::ostream& data, size_t size);

  /// Copy-free 'recv' of  'size' bytes, then return an iterator allowing you to
  /// 'spy' on the buffer. You must run consume after you're done looking at the
  /// data, to prevent repeats for the next spy operation.
  SpyGuard spy(size_t size);

  /// Copy-free 'recv' until at least 'delim'. The range returned is the whole
  /// buffer and may pass delim. You must use 'consume' after reading the number
  /// of bytes, or the buffer will remain full and the next spy call will see
  /// the same thing.
  SpyGuard spy(char delim);

  /// Wipe out the buffer we don't need any more after 'spy'ing it
  void consume(size_t size);
};

// A guard allowing you to see a line in the connection, then have it be
// consumed after use
struct SpyGuard {
  using Consume = std::function<void(size_t)>;
  Consume consume;
  Connection::SpyRange buf;
  SpyGuard(Consume consume, Connection::SpyRange buf)
      : consume(consume), buf(buf) {}
  SpyGuard(const SpyGuard &) = delete;
  SpyGuard(SpyGuard &&other) : consume(other.consume), buf(other.buf) {
    other.consume = {};
    other.buf = {};
  }
  ~SpyGuard() {
    if (consume)
      consume(buf.size());
  }
  SpyGuard &operator=(SpyGuard&& other) {
    // Eat our old buffer
    consume(buf.size());
    // Grab the new buffer
    consume = other.consume;
    buf = other.buf;
    return *this;
  }
  auto begin() const { return buf.begin(); }
  auto end() const { return buf.end(); }
  auto size() const { return buf.size(); }
  auto range() const { return buf; }
};

/// Allows us to extend a string with a boost::iterator_range
inline std::string& operator +=(std::string& a, Connection::SpyRange b) {
    std::string temp;
    std::copy(b.begin(), b.end(), std::back_inserter(a));
    return a;
}

/// Allows us to extend a string with a boost::iterator_range
inline std::string operator +(std::string a, Connection::SpyRange b) {
  std::string temp(a);
  temp += b;
  return temp;
}

/// Allows us to extend a string with a boost::iterator_range
inline std::string& operator +=(std::string& a, const SpyGuard& b) {
  return a += b.range();
}

/// Allows us to extend a string with a boost::iterator_range
inline std::string operator +(std::string a, const SpyGuard& b) {
  return a + b.range();
}

inline bool operator==(const SpyGuard &g, const std::string &s) {
  return s == g.range();
}

inline bool operator!=(const SpyGuard &g, const std::string &s) {
  return s != g.range();
}


} // tcpip
} /* RESTClient */
