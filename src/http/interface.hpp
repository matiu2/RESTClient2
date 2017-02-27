#pragma once

#include "Request.hpp"
#include "../tcpip/Connection.hpp"

#include <boost/asio/spawn.hpp>

namespace RESTClient {
namespace http {

using boost::asio::yield_context;

/// Send an HTTP request
/// Takes url and optional verb
inline Request request(tcpip::Connection &conn, const std::string &url) {
  return Request(conn, url);
}

inline Request get(tcpip::Connection &conn, const std::string &url) {
  return request(conn, url).verb("GET");
}

inline Request post(tcpip::Connection &conn, const std::string &url) {
  return request(conn, url).verb("POST");
}

inline Request put(tcpip::Connection &conn, const std::string &url) {
  return request(conn, url).verb("PUT");
}

inline Request patch(tcpip::Connection &conn, const std::string &url) {
  return request(conn, url).verb("PATCH");
}

inline Request delet(tcpip::Connection &conn, const std::string &url) {
  return request(conn, url).verb("DELETE");
}

/// Spawn a task that can create and call requests
/// This just uses boost::asio::spawn
void spawn(std::function<void(yield_context)>&& worker);

/// Run the main service (doesn't return until there's no more work to do)
/// See the docs for boost::asio::io_service::run for more info.
/// Returns the number of handlers run
size_t run();

} /* http */
} /* RESTClient  */

