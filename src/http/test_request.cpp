#include "interface.hpp"

#include <iostream>

using namespace RESTClient::http;
using namespace std;

void worker(yield_context yield) {
  cout << "Creating request...\n";
  Request req("http://httpbin.org/get");
  cout << "Sending request\n";
  Response res = req.go(yield);
  cout << "Done\n";
}

int main(int, char **) {
  spawn(worker);
  run();
  return 0;
}
