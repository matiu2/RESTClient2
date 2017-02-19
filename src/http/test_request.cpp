#include "interface.hpp"

#include <jsonpp11/parse_to_json_class.hpp>
#include <cassert>
#include <iostream>
#include <set>

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

void chunked_post(yield_context yield) {
  cout << "Running chunked_post..." << endl;
  std::string part("0123456789");
  // When we send a stream without a content length it'll send it with a
  // chunked transmit.
  // It'll send it in 1024 byte chunks, so we'll make a 1500 byte body
  std::stringstream body;
  for (int i = 0; i < 150; ++i)
    body << part;
  Response res = post("http://httpbin.org/post").body(body).go(yield);
  auto j = json::readValue(res.body.begin(), res.body.end());
  requireEqual(j["data"], body.str());
  cout << "Run chunked_post done" << endl;
}

void get_add_header(yield_context yield) {
  cout << "Running get_add_header..." << endl;
  Request req("http://httpbin.org/get");
  req.add_header("X-Test-Header", "Test Value");
  Response res = req.go(yield);
  auto body = json::readValue(res.body.begin(), res.body.end());
  requireEqual(body["url"], "http://httpbin.org/get");
  requireEqual(body["headers"]["X-Test-Header"], "Test Value");
  cout << "Run get_add_header done" << endl;
}

void doGet(yield_context yield) {
  cout << "Running get..." << endl;
  Request req("http://httpbin.org/get");
  Response res = req.go(yield);
  auto body = json::readValue(res.body.begin(), res.body.end());
  requireEqual(body["url"], "http://httpbin.org/get");
  cout << "Run get done" << endl;
}

void gzip(yield_context yield) {
  cout << "Running gzipped" << endl;
  Request req("http://httpbin.org/gzip");
  Response res = req.go(yield);
  auto body = json::readValue(res.body.begin(), res.body.end());
  requireEqual(res.headers["Content-Encoding"], "gzip");
  requireEqual(body["method"], "GET");
  assert((bool)body["gzipped"]);
  cout << "Run gzipped done" << endl;
}

void deflate(yield_context yield) {
  cout << "Running get_deflate..." << endl;
  Request req("http://httpbin.org/deflate");
  Response res = req.go(yield);
  auto body = json::readValue(res.body.begin(), res.body.end());
  requireEqual(res.headers["Content-Encoding"], "deflate");
  requireEqual(body["method"], "GET");
  assert((bool)body["deflated"]);
  cout << "Run get_deflate done" << endl;
}

void headers(yield_context yield) {
  cout << "Running headers..." << endl;
  Request req("http://httpbin.org/get");
  Response res = req.go(yield);
  cout << "Run headers done" << endl;
}

void doPost(yield_context yield) {
  cout << "Running post..." << endl;
  std::string body("This is the body baby");
  Response res = post("http://httpbin.org/post").body(body).go(yield);
  auto j = json::readValue(res.body.begin(), res.body.end());
  requireEqual(j["data"], body);
  cout << "Run post done" << endl;
}

int main(int argc, char **argv) {
  std::vector<std::string> testsToRun;
  if (argc > 1)
    testsToRun = {argv + 1, argv + argc};

  typedef void (*Action)(yield_context);

  std::map<std::string, Action> allTests{{"chunked_post", chunked_post},
                                         {"get_add_header", get_add_header},
                                         {"get", doGet},
                                         {"gzip", gzip},
                                         {"deflate", deflate},
                                         {"headers", headers},
                                         {"post", doPost}};

  if (!testsToRun.empty()) {
    for (const std::string &name : testsToRun) {
      auto found = allTests.find(name);
      if (found != allTests.end()) {
        cout << "Spawning " << name << endl;
        spawn(found->second);
      } else {
        cout << "Ignoring non-matching name: " << name << endl
             << "Possible test names: ";
        for (const auto &pair : allTests)
          cout << pair.first << ", ";
        cout << endl;
      }
    }
  } else {
    for (auto &pair : allTests) {
      cout << "Spawning " << pair.first << endl;
      spawn(pair.second);
    }
  }
  cout << run() << endl;
  return 0;
}

