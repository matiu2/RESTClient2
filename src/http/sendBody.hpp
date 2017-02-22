#pragma once

#include <ostream>
#include <streambuf>

#include "../tcpip/Connection.hpp"

namespace RESTClient {
namespace http {

const size_t buf_size = 1024;

void sendStream(std::istream &data,
                std::function<void(const std::string &)> sender) {
  std::string buf;
  buf.reserve(buf_size);
  std::istreambuf_iterator<char> in(data);
  std::istreambuf_iterator<char> end;
  while (in != end) {
    auto out = std::back_inserter(buf);
    // Copy from the stream into our buffer
    for (int i = 0; (i != buf_size) && (in != end); ++i, ++out, ++in)
      *out = *in;
    sender(buf);
    buf.clear();
  }
}

/// Performs http chunked encoding transmit of a stream (because we don't know
/// the content-length
void chunkedTransmit(std::istream& data, tcpip::Connection& conn) {
  sendStream(data, [&conn](const std::string &buf) {
    if (!buf.empty()) {
      // Never send an empty chunk, because that's what gets sent at the end
      std::stringstream len;
      len << std::hex << buf.size() << "\r\n";
      conn.send(len.str());
      conn.send(buf);
      conn.send("\r\n");
    }
  });
  // We need to send the 0 length chunk at the end
  conn.send("0\r\n\r\n");
}

/// Send a stream straight (having already sent the content-length)
void sendStream(std::istream &data, tcpip::Connection &conn) {
  sendStream(data, [&conn](const std::string &buf) { conn.send(buf); });
}

} /* http */ 
} /* RESTClient  */ 
