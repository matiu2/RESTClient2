#include "interface.hpp"

#include "jsonpp11/parse_to_json_class.hpp"
#include <iostream>
#include <cassert>

// From ../cmake/dependencies.cmake

using namespace RESTClient::http;
using namespace std;

void headers(yield_context yield) {
  Request req("http://httpbin.org/get");
  cout << "Sending request\n";
  Response res = req.go(yield);
  cout << "Done\n";
}

void get(yield_context yield) {
  Request req("http://httpbin.org/get");
  Response res = req.go(yield);
  using namespace std;
  cout << "Body: --------------------" << endl
       << res.body << endl
       << " --------------------- " << endl;
  auto body = json::readValue(res.body.begin(), res.body.end());
  assert(body["url"] == "http://httpbin.org/get");


  // Check the body. Should look like:
  // {
  //   "args" : {},
  //            "headers" : {
  //              "Accept" : "text/html,application/xhtml+xml,application/"
  //                         "xml;q=0.9,image/webp,*/*;q=0.8",
  //              "Accept-Encoding" : "gzip, deflate, sdch",
  //              "Accept-Language" : "en-US,en;q=0.8,es;q=0.6",
  //              "Host" : "httpbin.org",
  //              "Pragma" : "akamai-x-get-client-ip, akamai-x-cache-on, "
  //                         "akamai-x-cache-remote-on, akamai-x-check-cacheable, "
  //                         "akamai-x-get-cache-key, "
  //                         "akamai-x-get-extracted-values, akamai-x-get-nonces, "
  //                         "akamai-x-get-ssl-client-session-id, "
  //                         "akamai-x-get-true-cache-key, akamai-x-serial-no, "
  //                         "akamai-x-feo-trace, akamai-x-get-request-id",
  //              "Referer" : "http://httpbin.org/",
  //              "Upgrade-Insecure-Requests" : "1",
  //              "User-Agent" : "Mozilla/5.0 (X11; Linux x86_64) "
  //                             "AppleWebKit/537.36 (KHTML, like Gecko) "
  //                             "Chrome/55.0.2883.87 Safari/537.36"
  //            },
  //                        "origin" : "163.53.32.82",
  //                                  "url" : "http://httpbin.org/get"
  // }
  cout << "Done\n";
}

int main(int, char **) {
  spawn(::get);
  spawn(::headers);
  run();
  return 0;
}
