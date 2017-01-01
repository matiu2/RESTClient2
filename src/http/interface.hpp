#pragma once

#include "Request.hpp"

namespace RESTClient {
namespace http {

/// Send an HTTP request
/// Takes url and optional verb
Request request(const std::string &url, yield_context yield) { return Request(url, yield); }

} /* http */
} /* RESTClient  */

