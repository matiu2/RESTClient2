#include "interface.hpp"

#include <jsonpp11/parse_to_json_class.hpp>
#include <cassert>
#include <iostream>

// From ../cmake/dependencies.cmake

using namespace RESTClient::http;
using namespace std;

void headers(yield_context yield) {
  Request req("http://httpbin.org/get");
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

void post(yield_context yield) {
  std::string body("This is the body baby");
  Response res = post("http://httpbin.org/post").body(body).go(yield);
  using namespace std;
  auto j = json::readValue(res.body.begin(), res.body.end());
  assert(j["data"] == body);
  cout << "POST Done\n";
}

int main(int, char **) {
  using namespace std;
  cout << "Starting..." << endl;
  spawn(::get);
  spawn(::headers);
  spawn(::get_add_header);
  spawn(::post);
  run();
  return 0;
}
