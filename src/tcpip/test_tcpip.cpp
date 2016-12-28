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
  auto header = +(char_ - ':') >> ':' >> +(char_ - '\r') >> "\r\n";
  auto http = lit("HTTP/1.1");
  auto code = lexeme[char_("1-9") >> char_("0-9") >> char_("0-9")];
  auto code_line = http >> code >> +~char_('\r') >> "\r\n";
  auto ws = lit(' ');
} /* parser  */

void doWork(yield_context yield, Connection &&c) {
  std::string request("GET /get HTTP/1.1\r\n"
                      "Host: httpbin.org\r\n"
                      "X-My-Header: test21\r\n"
                      "\r\n");
  using namespace std;
  cout << request;
  c.send(request);

  // Read the code line
  auto line = c.spy('\n');
  auto i = line.begin();
  char code[4]("000");
  std::string ok;
  boost::fusion::tuple<std::string, std::string> code_line;
  auto out =  boost::fusion::vector_tie(code[0], code[1], code[2], ok);
  bool gotCodeLine = boost::spirit::x3::phrase_parse(
      i, line.end(), parser::code_line, parser::ws, out);

  if (!gotCodeLine) {
    std::string out;
    std::copy(line.begin(), line.end(), std::back_inserter(out));
    cerr << "Got stuck at position: " << std::distance(line.begin(), i) << "\r\n";
    cout << "Unable to parse code line: " << out;
    return;
  }

  cout << "Parsed " << std::distance(line.begin(), i)
       << " characters from the code line" << '\n';
  cout << "Got code: " << code << '\n' << "OK? " << ok << '\n';
  c.consume(std::distance(line.begin(), i));

  // Parse headers

  std::map<std::string, std::string> headers;

  while (true) {
    std::pair<std::string, std::string> header;
    auto out = boost::fusion::vector_tie(header.first, header.second);
    auto line = c.spy('\n');
    auto i = line.begin();
    bool got_header = boost::spirit::x3::phrase_parse(
        i, line.end(), parser::header, parser::ws, out);
    c.consume(std::distance(line.begin(), i));
    if (!got_header) {
      // Should be just a blank line
      if ((*i != '\r') && (*(i+1) != '\n')) {
        // If it's not a blank line, complain
        std::string brokenHeader("Couldn't parse an http error: ");
        std::copy(line.begin(), line.end(), std::back_inserter(brokenHeader));
        throw std::runtime_error(brokenHeader);
      }
      c.consume(2); // Consume the "\r\n"
      break;
    }
    cout << "Got Header: " << header.first << ": " << header.second << '\n';
    headers.emplace(std::move(header));
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
    doWork(yield, Connection("httpbin.org", "http", yield));
  });
  spawn(*io, [](yield_context yield) {
    doWork(yield, Connection("httpbin.org", "https", yield, true));
  });
  io->run();
  return 0;
}
