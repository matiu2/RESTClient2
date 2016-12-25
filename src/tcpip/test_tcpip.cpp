/// Tests that the tcpip connections work

#include "interface.hpp"

using namespace RESTClient::tcpip;

void doWork(yield_context yield) { Connection c("httpbin.org", "http", yield); }

int main(int, char **) {
  auto io = getService();
  spawn(*io, doWork);
  io->run();
  return 0;
}
