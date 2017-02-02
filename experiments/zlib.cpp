#include <zlib.h>

#include <string>
#include <iostream>

#include "../src/http/interface.hpp"

using namespace RESTClient::http;
using namespace std;

void gunzip(yield_context yield) {
  // Download gzipped data
  auto response = get("http://httpbin.org/gzip").go(yield);
  cout << response.body << endl;
}

int main(int argc, char *argv[]) {
  spawn(gunzip);
  run();
  return 0;
}
