/// Tests that the tcpip connections work

#include "interface.hpp"

using namespace RESTClient::tcpip;

void doWork(yield_context yield) {
  auto addresses = lookup("httpbin.org", "http", yield);


    
}

int main(int, char **) {
  auto io = getService();
  spawn(*io, doWork);
  io->run();
  return 0;
}
