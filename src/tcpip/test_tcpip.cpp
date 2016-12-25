/// Tests that the tcpip connections work

#include "interface.hpp"

#include <cassert>

using namespace RESTClient::tcpip;

void doWork(yield_context yield) {
  Connection c("httpbin.org", "http", yield);
  c.send("GET /get 1.1\r\nX-My-Header: test21\r\n\r\n");
  std::string line;
  c.recv(line);
  assert(line == "HTTP/1.1 200 OK\r\n");
  c.recv(line);
  assert(line == "Server: nginx\r\n");
}

int main(int, char **) {
  auto io = getService();
  spawn(*io, doWork);
  io->run();
  return 0;
}
