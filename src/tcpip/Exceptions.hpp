#pragma once

#include <boost/exception/all.hpp>
#include <boost/system/error_code.hpp>
#include <string>

using namespace std::string_literals;

namespace RESTClient {
namespace tcpip {

// Error info for the original 'what()'
using ei_orig = boost::error_info<struct tag_orig, std::string>;

/// Base exception for all tcpip errors
struct Exception : virtual std::exception, virtual boost::exception {};

/// Unable to resolve hostname
struct DNSLookupError : virtual Exception {};

// Error Info hostname - to add the hostname to a DNS lookup error
using ei_hostname = boost::error_info<struct tag_hostname, std::string>;

/// Unable to establish tcpip connection
struct ConnectionError : virtual Exception {};

// Error Info service - to add the service/port to a connect error
using ei_service = boost::error_info<struct tag_service, std::string>;

struct SSLShutdownError : virtual Exception {};

// Adding boost::system::error_code to an exception
using ei_errcode = boost::error_info<struct tag_syscode, boost::system::error_code>;

struct send_err : virtual Exception {};
struct recv_err : virtual Exception {};

// The number of bytes we tried to send, or received
using ei_numbytes = boost::error_info<struct tag_numbytes, unsigned short>;

// The number of bytes we were supposed to receiv
using ei_reqbytes = boost::error_info<struct tag_reqbytes, unsigned short>;

// The delimeter we were looking for
using ei_delim = boost::error_info<struct tag_delim, char>;

// The data we tried to send, or did recv
using ei_data = boost::error_info<struct tag_data, std::string>;

} /* tcpip */
} /* RESTClient  */
