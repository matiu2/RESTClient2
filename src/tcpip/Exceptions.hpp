#pragma once

#include <stdexcept>
#include <string>

using namespace std::string_literals;

namespace RESTClient {
namespace tcpip {

/// Base exception for all tcpip errors
struct Exception : public std::runtime_error {
  Exception(std::string what) : std::runtime_error(std::move(what)) {}
};

/// Unable to resolve hostname
struct DNSLookupError : public Exception {
  std::string hostname;
  std::string reason;
  DNSLookupError(std::string hostname, std::string reason)
      : Exception("DNS lookup error for '"s + hostname + ": " + reason),
        hostname(std::move(hostname)), reason(std::move(reason)) {}
};

/// Unable to establish tcpip connection
struct ConnectionError : public Exception {
  std::string hostname;
  std::string service;
  std::string reason;
  ConnectionError(std::string hostname, unsigned short port, std::string reason)
      : Exception("Unable to connect to '"s + hostname + "' service/port '"s +
                  std::to_string(port) + "': " + reason),
        hostname(std::move(hostname)), service(std::move(service)),
        reason(std::move(reason)) {}
  ConnectionError(const std::string &hostname, std::string service,
                  std::string reason)
      : Exception("Unable to connect to '"s + hostname + "' service/port '"s +
                  service + "': " + reason),
        hostname(std::move(hostname)), service(std::move(service))),
        reason(std::move(reason)) {}
};

struct SSLShutdownError : public Exception {
  SSLShutdownError(boost::system::error_code ec)
      : Exception("Unabled to shutdown SSL connection: "s +
                  ec.category().name() + " (" + std::to_string(ec.value()) +
                  ") " + ec.category().message(ec.value())) {}
};

} /* tcpip */ 
} /* RESTClient  */ 
