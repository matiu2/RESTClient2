#pragma once
/// A boost iostreams source for straight tcpip 

#include <iosfwd>                          // streamsize
#include <memory>
#include <boost/iostreams/categories.hpp>  // source_tag

#include "../tcpip/SpyGuard.hpp"

namespace RESTClient {
namespace http {

namespace io = boost::iostreams;

/// Spy n bytes of data from the asio buffers
using SpyN = std::function<tcpip::SpyGuard(size_t)>;
/// Spy until 'delim' is seen in the asio buffers
using SpyD = std::function<tcpip::SpyGuard(char)>;


class ChunkedSource {
public:
  typedef char char_type;
  typedef io::source_tag category;

  SpyN spyN;
  SpyD spyD;
  bool done = false;

  struct Innards {
    tcpip::SpyGuard chunk;
    tcpip::SpyIterator i = chunk.begin();
    bool done() const { return i == chunk.end(); }
  };

  std::shared_ptr<Innards> innards;

  ChunkedSource(SpyN spyN, SpyD spyD)
      : spyN(spyN), spyD(spyD) {
    assert(spyN);
    assert(spyD);
  }

  /// Fills the variables 'chunk' and 'i' with the next upcoming chunk
  void getNextChunk();

  std::streamsize read(char *s, std::streamsize n) {
    if (done)
      return -1;
    // Read up to n characters from the underlying data source
    // into the buffer s, returning the number of characters
    // read; return -1 to indicate EOF
    if ((!innards) || (innards->done()))
      getNextChunk();
    // There is no more data
    if (!innards) {
      // Need to set done, because they call back even after we tell them there's nothing left.
      done = true;
      return -1;
    }
    // There is data, return as much data as we can
    size_t count = 0;
    auto &chunk = innards->chunk;
    auto &i = innards->i;
    for (count = 0; (count != n) && (i != chunk.end()); ++count, ++s, ++i)
      *s = *i;
    return count;
  }
};

} /* http */ 
} /* RESTClient  */ 
