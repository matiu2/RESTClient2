#pragma once

#include <functional>
#include <boost/range/iterator_range.hpp>
#include <boost/asio/buffers_iterator.hpp>

namespace RESTClient {
namespace tcpip {

class Connection;

using SpyIterator = boost::asio::buffers_iterator<boost::asio::const_buffers_1>;
using SpyRange = boost::iterator_range<SpyIterator>;

// A guard allowing you to see a line in the connection, then have it be
// consumed after use
struct SpyGuard {
  using Consume = std::function<void(size_t)>;
  Consume consume;
  SpyRange buf;
  SpyGuard(Consume consume, SpyRange buf)
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
inline std::string& operator +=(std::string& a, SpyRange b) {
    std::string temp;
    std::copy(b.begin(), b.end(), std::back_inserter(a));
    return a;
}

/// Allows us to extend a string with a boost::iterator_range
inline std::string operator +(std::string a, SpyRange b) {
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
