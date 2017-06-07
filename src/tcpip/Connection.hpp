#pragma once

#include "../pimpl.hpp"
#include "SpyGuard.hpp"

#include <boost/asio/spawn.hpp>

#include <string>
#include <ostream>

namespace RESTClient {
namespace tcpip {

using boost::asio::yield_context;

struct SpyGuard;

class Connection {
public:
private:
  class impl;
  pimpl<impl> m;

public:
  Connection(std::string address, std::string service, yield_context yield,
             bool is_ssl = false);
  Connection(Connection &&other);
  ~Connection();
  /// Send data through the internet
  void send(const std::string &data);

  /// Receive exactly 'size' bytes and move it into a string
  void recv(std::string &data, size_t size);

  /// Receive until we hit some deliminator and move it into a string
  void recv(std::string &data, char delim);

  /// Receive a line and move it int a string
  void recv(std::string &data) { recv(data, '\n'); }

  /// Recieve 'n' bytes into a stream
  void recv(std::ostream &data, size_t size);

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

  yield_context &yield();
};

} // tcpip
} /* RESTClient */
