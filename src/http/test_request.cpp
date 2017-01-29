#include "interface.hpp"

#include <jsonpp11/parse_to_json_class.hpp>
#include <cassert>
#include <iostream>

// From ../cmake/dependencies.cmake

using namespace RESTClient::http;
using namespace std;

void headers(yield_context yield) {
  Request req("http://httpbin.org/get");
  cout << "Sending request\n";
  Response res = req.go(yield);
  cout << "Headers Done\n";
}

void get(yield_context yield) {
  Request req("http://httpbin.org/get");
  Response res = req.go(yield);
  using namespace std;
  auto body = json::readValue(res.body.begin(), res.body.end());
  assert(body["url"] == "http://httpbin.org/get");
  cout << "Get Done\n";
}

void get_add_header(yield_context yield) {
  Request req("http://httpbin.org/get");
  req.add_header("X-Test-Header", "Test Value");
  Response res = req.go(yield);
  using namespace std;
  auto body = json::readValue(res.body.begin(), res.body.end());
  assert(body["url"] == "http://httpbin.org/get");
  assert(body["headers"]["X-Test-Header"] == "Test Value");
  cout << "Get add_header Done\n";
}


int main(int, char **) {
  spawn(::get);
  spawn(::headers);
  spawn(::get_add_header);
  run();
  return 0;
}
