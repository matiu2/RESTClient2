#include "io_source_chunked.hpp"

#include <boost/spirit/home/x3.hpp>

using namespace std::string_literals;

namespace body_parser {
using namespace boost::spirit::x3;
auto chunk_line = ulong_ >> "\r\n";
} /* body_parser */

namespace RESTClient {
namespace http {

void ChunkedSource::getNextChunk() {
  size_t chunkSize;
  {
    // Curly braces needed because we kill the SpyGuard after we've read the
    // data we need
    tcpip::SpyGuard line(spyD('\n'));
    bool ok = boost::spirit::x3::phrase_parse(line.begin(), line.end(),
                                              body_parser::chunk_line,
                                              body_parser::space, chunkSize);
    if (!ok) {
      throw std::runtime_error("Couldn't parse chunk line: "s + line);
    }
  }
  assert(innards);
  if (chunkSize == 0) {
    // No more data
    innards.reset();
  } else {
    // Get a new chunk
    innards.reset(new Innards{spyN(chunkSize)});
  }
}

}
}
