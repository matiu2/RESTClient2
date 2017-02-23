#pragma once
/**
 * This is the main API file.
 *
 *
 */

#include <string>
#include "http/interface.hpp"

namespace RESTClient {

class REST {
private:
  std::string baseURL;
  RESTClient::http::Headers baseHeaders;

public:
  REST(std::string aBaseURL, RESTClient::http::Headers baseHeaders = {})
      : baseURL(std::move(aBaseURL)), baseHeaders(std::move(baseHeaders)) {
    assert(!baseURL.empty());
    // We don't want the '/' to be on the end of the baseURL; we add it on to
    // every request if needed
    if (baseURL.back() == '/')
      baseURL.erase(baseURL.end() - 1);
  }
  inline http::Request get(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::get(baseURL + '/' +  path).headers(baseHeaders);
    else
      return http::get(baseURL + path).headers(baseHeaders);
  }
  inline http::Request put(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::put(baseURL + '/' +  path).headers(baseHeaders);
    else
      return http::put(baseURL + path).headers(baseHeaders);
  }
  inline http::Request post(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::post(baseURL + '/' +  path).headers(baseHeaders);
    else
      return http::post(baseURL + path).headers(baseHeaders);
  }
  inline http::Request patch(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::patch(baseURL + '/' +  path).headers(baseHeaders);
    else
      return http::patch(baseURL + path).headers(baseHeaders);
  }
  inline http::Request delet(const std::string &path) {
    if ((path.empty()) || (path.front() != '/'))
      return http::delet(baseURL + '/' +  path).headers(baseHeaders);
    else
      return http::delet(baseURL + path).headers(baseHeaders);
  }
};

} /* RESTClient */

