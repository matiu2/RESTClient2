/// Tests the url_parser
#include "url_parser.hpp"

#include <cassert>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct Test {
  string url;
  string protocol;
  string username;
  string password;
  string hostname;
  string path;
  unsigned short port;
  map<string, string> params;

  RESTClient::URL parts;
  Test(string url, string protocol, string hostname, string path = "",
       string username = "", string password = "", unsigned short port = 0,
       map<string, string> params = {})
      : url(url), protocol(protocol), username(username), password(password),
        hostname(hostname), path(path), port(port), params(std::move(params)),
        parts(url) {
    if (port == 0) {
      if (protocol == "http") {
        this->port = 80;
      } else {
        this->port = 443;
        assert(protocol == "https");
      }
    }
  }
  bool test() const {
    cout << "Testing url: " << url << std::endl;
    std::stringstream msg;
    // Protocol
    if (protocol != parts.protocol) {
      msg << "Protocol doesn't match: expected (" << protocol << ") but found ("
          << parts.protocol << ")"
          << "\n";
    }
    // Username
    if (username != parts.username) {
      msg << "Username doens't match: expected (" << username << ") but found ("
          << parts.username << ")\n";
    }
    // Password
    if (password != parts.password) {
      msg << "Password doens't match: expected (" << password << ") but found ("
          << parts.password << ")\n";
    }
    // Hostname
    if (hostname != parts.hostname) {
      msg << "Hostname doens't match: expected (" << hostname << ") but found ("
          << parts.hostname << ")\n";
    }
    // Port
    if (port != parts.port) {
      msg << "Port doens't match: expected (" << port << ") but found ("
          << parts.port << ")\n";
    }
    // Path
    if (path != parts.path) {
      msg << "Path doens't match: expected (" << path << ") but found ("
          << parts.path << ")\n";
    }
    // Check each param
    bool print_params = false;
    if (params.size() != parts.params.size()) {
      msg << "We have a different number of params; expected (" << params.size()
          << ") but got (" << parts.params.size() << ")\n";
      print_params = true;
    }

    for (const auto &p1 : params) {
      const auto &key = p1.first;
      const auto &val = p1.second;
      auto found = parts.params.find(key);
      if (found == parts.params.end()) {
        msg << "Param (" << key << ") not found\n";
        print_params = true;
      }
      if (found->second != val) {
        msg << "Parama (" << key << ") had different value; expected (" << val
            << ") but got (" << found->second << ")\n";
        print_params = true;
      }
    }

    if (print_params) {
      msg << "\nExpected params: \n";
      for (const auto& param : params)
        msg << param.first << "=" << param.second << "\n";

      msg << "\nGotten params: \n";
      for (const auto& param : parts.params)
        msg << param.first << "=" << param.second << "\n";
    }

    // Now get the url back out of it
    auto new_url = parts.url();
    if (url != new_url) {
      msg << "URL (" << url << ") didn't convert back the same (" << new_url
          << ")\n";
    }


    // All done
    std::string out = msg.str();
    if (out.size() > 0) {
      cerr << "Test failed foraurl (" << url << ":\n" << msg.str() << '\n';
      return false;
    }
    return true;
  }
};

int main(int, char *[]) {
  std::vector<Test> tests{
      Test("http://google.com", "http", "google.com"),
      Test("https://google.com", "https", "google.com"),
      Test("https://google.com/", "https", "google.com", "/"),
      Test("https://www.google.com/something", "https", "www.google.com",
           "/something"),
      Test("https://www.google.com/something", "https", "www.google.com",
           "/something"),
      Test("http://user1@gmail.com?x=1&y=2", "http", "gmail.com", "", "user1",
           "", 80, {{"x", "1"}, {"y", "2"}}),
      Test("https://user1:pass2@gmail.com:8043/execute/awesomeness?x=1&y=2",
           "https", "gmail.com", "/execute/awesomeness", "user1", "pass2", 8043,
           {{"x", "1"}, {"y", "2"}})
  };
  for (const Test& test : tests)
    test.test();

  return 0;
}
