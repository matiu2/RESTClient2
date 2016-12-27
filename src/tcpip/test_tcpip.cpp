/// Tests that the tcpip connections work

#include "interface.hpp"

#include <boost/spirit/home/x3.hpp>

#include <cassert>
#include <iostream>

using namespace RESTClient::tcpip;

namespace parser {
  using namespace boost::spirit::x3;
  auto header = +(char_ - ':') >> ':' >> +(char_ - '\r') >> "\r\n";
} /* parser  */

void doWork(yield_context yield) {
  Connection c("httpbin.org", "http", yield);
  std::string request("GET /get HTTP/1.1\r\n"
                      "Host: httpbin.org\r\n"
                      "X-My-Header: test21\r\n"
                      "\r\n");
  using namespace std;
  cout << "> " << request;
  c.send(request);
  std::map<std::string, std::string> headers;
  std::string returnLine;
  for (int i=0; i<20; ++i) {
    c.recv(returnLine, '\n');
    cout << "Return line: " << returnLine << std::endl;
    returnLine.clear();
  }
  while (true) {
    std::pair<std::string, std::string> header;
    auto line = c.spy('\n');
    auto eol = line.begin();
    bool got_header = boost::spirit::x3::phrase_parse(
        eol, line.end(), parser::header, parser::space);
    c.consume(std::distance(line.begin(), eol));
    if (!got_header)
      break;
    headers.insert(header);
  }
}

int main(int, char **) {
  auto io = getService();
  spawn(*io, doWork);
  io->run();
  return 0;
}
