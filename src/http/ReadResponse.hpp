#pragma once

#include "../tcpip/interface.hpp"
#include "Response.hpp"

#include <ostream>

namespace RESTClient {
namespace http {

void readResponse(tcpip::Connection& conn, Response &out);
void readResponse(tcpip::Connection& conn, Response &out, std::ostream& body);

} /* http */
} /* RESTClient */
