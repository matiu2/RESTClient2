#include "ConnectionPool.hpp"

#include "url.hpp"

#include <string>

namespace RESTClient {
namespace http {

thread_local std::map<std::string, std::shared_ptr<tcpip::Connection>> pool;

std::shared_ptr<tcpip::Connection>
getConnection(const std::string &url_host_part, yield_context yield) {
  std::shared_ptr<tcpip::Connection> result;
  auto found = pool.find(url_host_part);
  if (found == pool.end()) {
    // Create the connections
    URL parts(url_host_part);
    result.reset(new tcpip::Connection(
        parts.hostname, std::to_string(parts.port), yield, parts.is_ssl()));
  } else {
    result = found->second;
  }
  return result;
}

} /* http */
} /* RESTClient  */
