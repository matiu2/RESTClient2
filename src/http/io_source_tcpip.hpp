#pragma once
/// A boost iostreams source for straight tcpip 

#include <iosfwd>                          // streamsize
#include <boost/iostreams/categories.hpp>  // source_tag
#include <algorithm>

#include <iostream>

#include "../tcpip/SpyGuard.hpp"

namespace RESTClient {
namespace http {

namespace io = boost::iostreams;

/// Max size we'll download at a time in bytes
const std::streamsize maxBlockSize = 8192;


class TCPIPSource {
public:
  using char_type = char;
  using category = io::source_tag;

  struct Innards {
    tcpip::SpyGuard data;
    tcpip::SpyIterator i = data.begin();
  };

  // We need a shared_ptr because boost::iostreams require sources to be
  // copyable
  std::shared_ptr<Innards> innards;

  TCPIPSource(tcpip::SpyGuard &&data) : innards(new Innards{std::move(data)}) {}

  std::streamsize read(char *s, std::streamsize n) {
    // Read up to n characters from the underlying data source
    // into the buffer s, returning the number of characters
    // read; return -1 to indicate EOF
    if (innards->i == innards->data.end())
      return -1;
    size_t count = 0;
    size_t max = std::min(maxBlockSize, n);
    auto& i = innards->i;
    auto& data = innards->data;
    for (count = 0; (count != max) && (i != data.end());
         ++count, ++s, ++i)
      *s = *i;
    return count;
  }
};

} /* http */ 
} /* RESTClient  */ 
