#include "Request.hpp"

#include "url.hpp"
#include "ConnectionPool.hpp"

namespace RESTClient {
namespace http {

Response Request::go() const {
  Response result;
  /// Get a TCP/IP connection from the connection pool
  URL url(this->url);
  auto conn = getConnection(url.host_part(), yield);
  /// Send the request line

  return result;
}

} /* http */
} /* RESTClient  */
