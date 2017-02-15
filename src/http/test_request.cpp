#include "interface.hpp"

#include <jsonpp11/parse_to_json_class.hpp>
#include <cassert>

// From ../cmake/dependencies.cmake

using namespace RESTClient::http;
using namespace std;

void requireEqual(const std::string& a, const std::string& b) {
  if (a != b) {
    std::stringstream msg;
    msg << a << " != " << b;
    throw std::runtime_error(msg.str());
  }
}

void headers(yield_context yield) {
  cout << "headers..." << endl;
  Request req("http://httpbin.org/get");
  Response res = req.go(yield);
  cout << "Headers Done\n";
}

void get(yield_context yield) {
  cout << "get..." << endl;
  Request req("http://httpbin.org/get");
  Response res = req.go(yield);
  using namespace std;
  auto body = json::readValue(res.body.begin(), res.body.end());
  requireEqual(body["url"], "http://httpbin.org/get");
  cout << "Get Done\n";
}

void get_add_header(yield_context yield) {
  cout << "get_add_header..." << endl;
  Request req("http://httpbin.org/get");
  req.add_header("X-Test-Header", "Test Value");
  Response res = req.go(yield);

  auto body = json::readValue(res.body.begin(), res.body.end());

  requireEqual(body["url"], "http://httpbin.org/get");
  requireEqual(body["headers"]["X-Test-Header"], "Test Value");

  cout << "Get add_header Done\n";
}

void post(yield_context yield) {
  cout << "post..." << endl;
  std::string body("This is the body baby");
  Response res = post("http://httpbin.org/post").body(body).go(yield);

  auto j = json::readValue(res.body.begin(), res.body.end());
  requireEqual(j["data"], body);
  cout << "POST Done\n";
}

void chunked_post(yield_context yield) {
  cout << "chunked_post..." << endl;
  std::string part("0123456789");
  // When we send a stream without a content length it'll send it with a chunked transmit.
  // It'll send it in 1024 byte chunks, so we'll make a 1500 byte body
  std::stringstream body;
  for (int i=0; i < 150; ++i)
    body << part;
  Response res = post("http://httpbin.org/post").body(body).go(yield);
  using namespace std;
  auto j = json::readValue(res.body.begin(), res.body.end());
  requireEqual(j["data"], body.str());
  cout << "Chunked POST Done\n";
}


int main(int, char **) {
  using namespace std;
  cout << "Starting..." << endl;
  spawn(::get);
  spawn(::headers);
  spawn(::get_add_header);
  spawn(::post);
  spawn(::chunked_post);
  run();
  return 0;
}
