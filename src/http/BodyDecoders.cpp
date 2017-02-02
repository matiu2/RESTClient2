#include "BodyDecoders.hpp"

#include <boost/spirit/home/x3.hpp>

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

size_t getChunkSize(GetLine getLine)  {
  size_t chunkSize;
  std::string line;
  getLine(line);
  bool ok = boost::spirit::x3::phrase_parse(line.begin(), line.end(), body_parser::chunk_line, body_parser::space, chunkSize);
  if (!ok) {
    throw std::runtime_error("Couldn't parse chunk line: "s  + line);
  }
  return chunkSize;
}

size_t readChunk(GetLine getLine, GetN<std::string> getN, std::string &out, bool clear) {
  size_t chunkSize = getChunkSize(getLine);
  if (clear)
    out.clear();
  if (chunkSize != 0) {
    out.reserve(chunkSize);
    getN(chunkSize, out);
  }
  return chunkSize;
}

void chunked(GetLine getLine, GetN<std::string> getN, std::string& out) {
  size_t chunkSize;
  do {
    chunkSize = readChunk(getLine, getN, out, false);
  } while (chunkSize != 0);
}

void chunked(GetLine getLine, GetN<std::ostream> getN, std::ostream& out) {
  size_t chunkSize;
  do {
    chunkSize = getChunkSize(getLine);
    getN(chunkSize, out);
  } while (chunkSize != 0);
}

void zipped(GetLine getLine, GetN<std::string> getN, std::string& out) {
}

void zipped(GetLine getLine, GetN<std::ostream> getN, std::ostream& out) {
}



} /* decoder  */ 
} /* http  */ 
} /* RESTClient  */ 
