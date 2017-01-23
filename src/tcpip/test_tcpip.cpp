/// Tests that the tcpip connections work

#include "interface.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/vector_tie.hpp>
#include <boost/fusion/tuple.hpp>

#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>

using namespace RESTClient::tcpip;

namespace parser {
  using namespace boost::spirit::x3;
  auto firstLine = lit("HTTP/1.1") >> ushort_ >> +(char_ - '\r') >> "\r\n";
  auto header = +(char_ - ':') >> ':' >> +(char_ - '\r') >> "\r\n";
  auto spacer = (lit(' ') | lit('\t'));
} /* parser  */

void doWork(Connection &&c) {
  std::string request("GET /get HTTP/1.1\r\n"
                      "Host: httpbin.org\r\n"
                      "X-My-Header: test21\r\n"
                      "\r\n");
  using namespace std;
  cout << request;
  c.send(request);

  // Read the code line
  {
    auto line = c.spy('\n');
    unsigned short code;
    std::string ok;
    auto out = boost::fusion::vector_tie(code, ok);
    bool gotCodeLine = boost::spirit::x3::phrase_parse(
        line.begin(), line.end(), parser::firstLine, parser::spacer, out);

    if (!gotCodeLine) {
      cerr << "Got stuck at position: "
           << std::distance(line.begin(), line.end()) << "\r\n";
      cout << "Unable to parse code line: "s + line;
      return;
    }

    cout << "Got code: " << code << '\n' << "OK? " << ok << '\n';
  }

  // Parse headers

  std::map<std::string, std::string> headers;

  while (true) {
    std::string key, val;
    auto out = boost::fusion::vector_tie(key, val);
    auto line = c.spy('\n');
    bool got_header = boost::spirit::x3::phrase_parse(
        line.begin(), line.end(), parser::header, parser::spacer, out);
    if (!got_header) {
      // Should be just a blank line
      if (line != "\r\n"s) {
        // If it's not a blank line, complain
        std::string brokenHeader("Couldn't parse an http error: ");
        std::copy(line.begin(), line.end(), std::back_inserter(brokenHeader));
        throw std::runtime_error(brokenHeader);
      }
      break;
    }
    cout << "Got Header: " << key << ": " << val << '\n';
    headers.insert({std::move(key), std::move(val)});
  }

  // Find the content length
  size_t contentLength = std::atoi(headers["Content-Length"].c_str());

  // Get the body
  std::string body;
  c.recv(body, contentLength);
  
  cout << "got the body: " << '\n' << body << '\n';

}

int main(int, char **) {
  auto io = getService();
  spawn(*io, [](yield_context yield) {
    doWork(Connection("httpbin.org", "http", yield));
  });
  spawn(*io, [](yield_context yield) {
    doWork(Connection("httpbin.org", "https", yield, true));
  });
  io->run();
  return 0;
}
