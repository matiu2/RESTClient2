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
        hostname(hostname), port(port), params(params), path(path), parts(url) {
    if (port == 0) {
      if (protocol == "http") {
        port = 80;
      } else {
        port = 443;
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
    // Hostname
    if (hostname != parts.hostname) {
      msg << "Hostname doens't match: expected (" << hostname << ") but found ("
          << parts.hostname << ")\n";
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
           "", 80, {{"x", "1"}, {"y", "2"}})};
  for (const Test& test : tests)
    test.test();

  return 0;
}
