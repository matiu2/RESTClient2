#include "ReadResponse.hpp"

#include "../tcpip/interface.hpp"
#include "io_source_tcpip.hpp"
#include "io_source_chunked.hpp"

#include <string>
#include <sstream>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/vector_tie.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>

using namespace std::string_literals;

namespace header_parser {

using namespace boost::spirit::x3;

auto firstLine = lit("HTTP/1.1") >> ushort_ >> +(char_ - '\r') >> "\r\n";
auto header =
    lexeme[+(char_ - ':')] >> ':' >> lexeme[+(char_ - '\r')] >> "\r\n";
auto spacer = (lit(' ') | lit('\t'));

} /* header_parser */ 

namespace RESTClient {
namespace http {

bool is_chunked(const Headers &headers) {
  auto found = headers.find("Transfer-Encoding");
  if (found == headers.end())
    return found->second == "chunked";
  else
    return false;
}

bool is_deflated(const Headers &headers) {
  auto found = headers.find("Content-Encoding");
  if (found == headers.end())
    return found->second == "deflate";
  else
    return false;
}

size_t getContentLength(const Headers& headers) {
  auto found = headers.find("Content-Length");
  if (found != headers.end())
    return std::stoul(found->second);
  // If there is no contentLength, the decoder shouldn't be calling this
  std::stringstream msg;
  msg << "There is no contentLength in these headers: \n";
  for (const auto& pair : headers) {
    msg << pair.first << ": " << pair.second << '\n';
  }
  throw std::runtime_error(msg.str());
}

/// Allows us to extend a string with a boost::iterator_range
template <typename T>
std::string& operator +=(std::string& a, boost::iterator_range<T> b) {
    std::string temp;
    std::copy(b.begin(), b.end(), std::back_inserter(a));
    return a;
}

/// Allows us to extend a string with a boost::iterator_range
template <typename T>
std::string operator +(std::string a, boost::iterator_range<T> b) {
    a += b;
    return a;
}

void readHeadersPart(tcpip::Connection &conn, Response &out) {
  // Now we read the reply .. first the headers

  // Read the first line
  {
    auto firstLineOut = boost::fusion::vector_tie(out.code, out.ok);
    auto line = conn.spy('\n');
    bool ok = boost::spirit::x3::phrase_parse(
        line.begin(), line.end(), header_parser::firstLine,
        header_parser::spacer, firstLineOut);
    if (!ok) {
      throw std::runtime_error("Unable to parse first line: '"s + line + "'");
    }
  }

  // Read the headers
  while (true) {
    std::string key, val;
    auto parsed = boost::fusion::vector_tie(key, val);
    auto line = conn.spy('\n');
    if (line == "\r\n")
      break;
    bool ok = boost::spirit::x3::phrase_parse(line.begin(), line.end(),
                                              header_parser::header,
                                              header_parser::spacer, parsed);
    if (!ok) {
      throw std::runtime_error("Unable to parse header: '"s + line + "'");
    }
    out.headers.emplace(std::move(key), std::move(val));
  }

  // Now check what sort of body encoding we have
  auto found = out.headers.find("Transfer-Encoding");
  std::string encoding("identity");
  if (found != out.headers.end())
    encoding = found->second;
}

void initBodyStream(tcpip::Connection &conn, Response &out, io::filtering_istream& bodyStream) {
  // Now see if it needs unzipping
  auto found = out.headers.find("Content-Encoding");
  if (found != out.headers.end()) {
    if (found->second == "gzip")
      bodyStream.push(io::gzip_decompressor());
    else if (found->second == "deflate")
      bodyStream.push(io::zlib_decompressor());
    // TODO: what if it has some unknown encoding ? add more decoders ?
  }

  // Now choose a source
  if (is_chunked(out.headers)) {
    auto spy = [&conn](auto x) { return std::move(conn.spy(x)); };
    bodyStream.push(ChunkedSource(spy, spy));
  } else {
    size_t len = getContentLength(out.headers);
    bodyStream.push(TCPIPSource(conn.spy(len)));
  }
}

void readResponse(tcpip::Connection &conn, Response &out) {
  readHeadersPart(conn, out);

  // TODO: If no decoding is required, just copy straight from asio buffers to
  // 'out' like how the headers are read
  io::filtering_istream bodyStream;
  initBodyStream(conn, out, bodyStream);

  // Now copy the de-chunked and unzipped data to the usable body
  std::noskipws(bodyStream);
  std::copy(std::istream_iterator<char>(bodyStream),
            std::istream_iterator<char>(), std::back_inserter(out.body));

}

void readResponse(tcpip::Connection &conn, Response &out, std::ostream &body) {
  readHeadersPart(conn, out);
  // TODO: If no decoding is required, just copy straight from asio buffers to
  // 'out' like how the headers are read
  io::filtering_istream bodyStream;
  initBodyStream(conn, out, bodyStream);

  // Now copy the de-chunked and unzipped data to the usable body
  std::copy(std::istream_iterator<char>(bodyStream),
            std::istream_iterator<char>(), std::ostream_iterator<char>(body));
}

} /* http */
} /* RESTClient */
