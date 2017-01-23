#pragma once

#include "Request.hpp"

#include <boost/asio/spawn.hpp>

namespace RESTClient {
namespace http {

using boost::asio::yield_context;

/// Send an HTTP request
/// Takes url and optional verb
Request request(const std::string &url) { return Request(url); }

/// Spawn a task that can create and call requests
void spawn(std::function<void(yield_context)> worker);

/// Run the main service (doesn't return until there's no more work to do)
/// See the docs for boost::asio::io_service::run for more info.
void run();

} /* http */
} /* RESTClient  */

