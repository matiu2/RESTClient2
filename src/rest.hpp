#pragma once
/**
 * This is the main API file.
 *
 *
 */

#include <string>
#include "http/interface.hpp"
#include "http/url.hpp"

namespace RESTClient {

class REST {
private:
  std::string baseURL;
  RESTClient::http::Headers baseHeaders;
  tcpip::Connection conn;

public:
  REST(http::yield_context &yield, http::URL baseURL,
       RESTClient::http::Headers baseHeaders = {})
      : baseURL(baseURL.whole()), baseHeaders(std::move(baseHeaders)),
        conn(baseURL.hostname, baseURL.protocol, yield, baseURL.is_ssl()) {
    // We don't want the '/' to be on the end of the baseURL; we add it on to
    // every request if needed
    while ((baseURL.whole().size() > 0) && (this->baseURL.back() == '/'))
      this->baseURL.erase(this->baseURL.end() - 1);
  }
  inline http::Request get(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::get(conn, baseURL + '/' + path).headers(baseHeaders);
    else
      return http::get(conn, baseURL + path).headers(baseHeaders);
  }
  inline http::Request head(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::head(conn, baseURL + '/' + path).headers(baseHeaders);
    else
      return http::head(conn, baseURL + path).headers(baseHeaders);
  }
  inline http::Request put(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::put(conn, baseURL + '/' + path).headers(baseHeaders);
    else
      return http::put(conn, baseURL + path).headers(baseHeaders);
  }
  inline http::Request post(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::post(conn, baseURL + '/' + path).headers(baseHeaders);
    else
      return http::post(conn, baseURL + path).headers(baseHeaders);
  }
  inline http::Request patch(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::patch(conn, baseURL + '/' + path).headers(baseHeaders);
    else
      return http::patch(conn, baseURL + path).headers(baseHeaders);
  }
  inline http::Request delet(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::delet(conn, baseURL + '/' + path).headers(baseHeaders);
    else
      return http::delet(conn, baseURL + path).headers(baseHeaders);
  }
};

} /* RESTClient */

