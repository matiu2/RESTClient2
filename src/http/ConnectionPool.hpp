#pragma once
/// Handles a pool of http connections that can be re-used

#include "../tcpip/interface.hpp"

namespace RESTClient {
namespace http {

using boost::asio::yield_context;

std::shared_ptr<tcpip::Connection>
getConnection(const std::string &url_host_part, yield_context yield);

} /* http */
} /* RESTClient */
