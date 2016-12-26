#pragma once

#include "../pimpl.hpp"
#include "interface.hpp"
#include <boost/asio/spawn.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include <string>

namespace RESTClient {
namespace tcpip {

using boost::asio::yield_context;

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
             bool is_ssl = true);
  ~Connection();
  void send(std::string data);
  /// Recv exactly 'size' bytes
  void recv(std::string& data, std::size_t size);
  /// Receive until we hit some deliminator
  void recv(std::string& data, char delim);
  /// Receive a line
  void recv(std::string& data) { recv(data, '\n'); }
  /// Copy-free 'recv' of  'size' bytes, then return an iterator allowing you to
  /// 'spy' on the buffer. You must run consume after you're done looking at the
  /// data, to prevent repeats for the next spy operation.
  SpyRange spy(size_t size);
  /// Copy-free 'recv' until at least 'delim'. The range returned is the whole
  /// buffer and may pass delim. You must use 'consume' after reading the number
  /// of bytes, or the buffer will remain full and the next spy call will see
  /// the same thing.
  SpyRange spy(char delim);
  /// Wipe out the buffer we don't need any more after 'spy'ing it
  void consume(size_t size);
};

} // tcpip
} /* RESTClient */
