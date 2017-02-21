#include "io_source_chunked.hpp"

#include <boost/spirit/home/x3.hpp>

#ifdef DEBUG_CHUNK_DECODER
#include <iostream>
#include <iterator>
#endif

using namespace std;

using namespace std::string_literals;

namespace body_parser {
using namespace boost::spirit::x3;
auto chunk_line = lexeme[boost::spirit::x3::hex >> "\r\n"];
} /* body_parser */

namespace RESTClient {
namespace http {

void ChunkedSource::getNextChunk() {
  size_t chunkSize = 0;
  if (innards) {
    // Delete any old spy guards to consume the buffers before reding again
    assert(innards->done());
    innards.reset();
    // Read the empty line before the chunk size and ignore it
    tcpip::SpyGuard line(spyD('\n'));
  }
  {
    // Curly braces needed because we kill the SpyGuard after we've read the
    // data we need
    tcpip::SpyGuard line(spyD('\n'));
    bool ok = boost::spirit::x3::phrase_parse(line.begin(), line.end(),
                                              body_parser::chunk_line,
                                              body_parser::space, chunkSize);
    if (!ok) {
      #ifdef DEBUG_CHUNK_DECODER
      cout << "Read chunk size fail: "s + line << endl;
      #endif
      throw std::runtime_error("Couldn't parse chunk line: "s + line);
    }
    #ifdef DEBUG_CHUNK_DECODER
    cout << "Read chunk size: " << chunkSize << endl;
    #endif
  }
  if (chunkSize != 0) {
    // Get a new chunk
    innards.reset(new Innards{spyN(chunkSize)});
    // TODO: Maybe add a cmake option to turn DEBUG_CHUNK_DECODER on and off
    #ifdef DEBUG_CHUNK_DECODER
    std::string debug;
    std::copy(innards->chunk.begin(), innards->chunk.end(), std::back_inserter(debug));
    cout << endl << "==================================" << endl << debug << endl << "======================================" << endl;
    #endif
  }
}

}
}
