#include "url_parser.hpp"

#include <string>

using namespace std;

struct Test {
  string url;
  string protocol;
  string host;
  string port;
  string rest;

  URL parts;
  Test(string url, string protocol, string hostname, string rest)
      : url(url), parts(url) {}
};

int main(int argc, char *argv[]) {
  Test a("http://google.com", "http", "google.com", "");
  Test b("https://google.com", "https", "google.com", "");
  Test c("https://google.com/", "https", "google.com", "/");
  Test d("https://www.google.com/something", "https", "www.google.com", "/something");
  Test e("https://www.google.com/something", "https", "www.google.com", "/something");
  
  
  return 0;
}
