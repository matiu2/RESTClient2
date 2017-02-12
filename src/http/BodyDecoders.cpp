#include "BodyDecoders.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <unordered_map>
#include <iterator>
#include <string>

using namespace std::string_literals;

namespace body_parser {
using namespace boost::spirit::x3;
auto chunk_line = ulong_ >> "\r\n";
} /* body_parser */

namespace RESTClient {
namespace http {
namespace decoder {

size_t getChunkSize(tcpip::SpyIterator& begin, SpyIterator end)  {
  size_t chunkSize;
  auto start = begin;
  bool ok = boost::spirit::x3::phrase_parse(begin, end, body_parser::chunk_line,
                                            body_parser::space, chunkSize);
  if (!ok) {
    std::string line;
    std::copy(start, end, std::back_inserter(line));
    throw std::runtime_error("Couldn't parse chunk line: "s + line);
  }
  return chunkSize;
}

/// Tries to copy 'n' bytes
size_t copyn(tcpip::SpyIterator& in, tcpip::SpyIterator end, size_t n) {
   
}

size_t readChunk(tcpip::SpyGuard &spyGuard, std::string &out) {
  auto in = spyGuard.begin();
  size_t chunkSize = getChunkSize(in, spyGuard.end());
  if (chunkSize != 0) {
    out.reserve(chunkSize);
    getN(chunkSize, out);
  }
  return chunkSize;
}

void getLine(tcpip::SpyGuard &guard) {
}

void chunked(GetSpyGuard getSpyGuard, std::string& out) {
  size_t chunkSize;
  do {
    chunkSize = readChunk(getLine, getN, out, false);
  } while (chunkSize != 0);
}

void chunked(GetSpyGuard getSpyGuard, std::ostream& out) {
  size_t chunkSize;
  do {
    chunkSize = getChunkSize(getLine);
    getN(chunkSize, out);
  } while (chunkSize != 0);
}

} /* decoder  */ 
} /* http  */ 
} /* RESTClient  */ 
