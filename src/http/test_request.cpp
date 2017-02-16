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

void headers(yield_context yield) {
  Request req("http://httpbin.org/get");
  Response res = req.go(yield);
}

void get(yield_context yield) {
  Request req("http://httpbin.org/get");
  Response res = req.go(yield);
  auto body = json::readValue(res.body.begin(), res.body.end());
  requireEqual(body["url"], "http://httpbin.org/get");
}

void get_add_header(yield_context yield) {
  Request req("http://httpbin.org/get");
  req.add_header("X-Test-Header", "Test Value");
  Response res = req.go(yield);

  auto body = json::readValue(res.body.begin(), res.body.end());

  requireEqual(body["url"], "http://httpbin.org/get");
  requireEqual(body["headers"]["X-Test-Header"], "Test Value");

}

void get_gzipped(yield_context yield) {
  Request req("http://httpbin.org/gzip");
  Response res = req.go(yield);
  auto body = json::readValue(res.body.begin(), res.body.end());
  requireEqual(body["method"], "GET");
  assert((bool)body["gzipped"]);
}

void post(yield_context yield) {
  std::string body("This is the body baby");
  Response res = post("http://httpbin.org/post").body(body).go(yield);

  auto j = json::readValue(res.body.begin(), res.body.end());
  requireEqual(j["data"], body);
}

void chunked_post(yield_context yield) {
  std::string part("0123456789");
  // When we send a stream without a content length it'll send it with a chunked transmit.
  // It'll send it in 1024 byte chunks, so we'll make a 1500 byte body
  std::stringstream body;
  for (int i=0; i < 150; ++i)
    body << part;
  Response res = post("http://httpbin.org/post").body(body).go(yield);
  auto j = json::readValue(res.body.begin(), res.body.end());
  requireEqual(j["data"], body.str());
}

int main(int argc, char **argv) {
  std::set<std::string> testsToRun;
  if (argc > 1)
    testsToRun = {argv + 1, argv + argc};

  std::map<std::string, std::function<void(yield_context)>> tests{
      {"chunked_post", chunked_post},
      {"get_add_header", get_add_header},
      {"get", ::get},
      {"get_gzipped", get_gzipped},
      {"headers", headers},
      {"post", ::post}};

  if (!testsToRun.empty()) {
    for (const std::string &name : testsToRun) {
      cout << "Spawning " << name << endl;
      auto found = tests.find(name);
      if (found != tests.end()) {
        spawn(tests[name]);
      } else {
        cout << "Ignoring non-matching name: " << name << endl << "Possible test names: ";
        for (const auto& pair : tests)
          cout << pair.first << ", ";
        cout << endl;
      }
    }
  } else {
    for (const auto &pair : tests) {
      cout << "Spawning " << pair.first << endl;
      spawn(pair.second);
    }
  }
  spawn(::get);
  spawn(headers);
  spawn(get_add_header);
  spawn(get_gzipped);
  spawn(::post);
  spawn(chunked_post);
  run();
  return 0;
}

