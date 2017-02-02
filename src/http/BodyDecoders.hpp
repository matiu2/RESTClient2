#pragma once
/* These are the decoders we should eventually support
 *
 * +--------------+---------------------------------------------------------+------------------------------------------+---------------+
 * |     Name     |                       Description                       |                Reference                 |     Notes     |
 * +--------------+---------------------------------------------------------+------------------------------------------+---------------+
 * |   chunked    |              Transfer in a series of chunks             |                [RFC7230]                 |  Section 4.1  |
 * |      br      |              Brotli Compressed Data Format              |                [RFC7932]                 |               |
 * |   compress   |               UNIX "compress" data format               |                [RFC7230]                 | Section 4.2.1 |
 * |   deflate    |              "deflate" zlib compressed data             |       [RFC1951][RFC1950][RFC7230]        | Section 4.2.2 |
 * |     exi      |              W3C Efficient XML Interchange              | [Efficient XML Interchange (EXI) Format] |               |
 * |     gzip     |                     GZIP file format                    |            [RFC1952][RFC7230]            | Section 4.2.3 |
 * |   identity   | Reserved (synonym for "no encoding" in Accept-Encoding) |                [RFC7231]                 | Section 5.3.4 |
 * | pack200-gzip |        Network Transfer Format for Java Archives        |                [JSR 200]                 |               |
 * |  x-compress  |             Deprecated (alias for compress)             |                [RFC7230]                 | Section 4.2.1 |
 * |    x-gzip    |               Deprecated (alias for gzip)               |                [RFC7230]                 | Section 4.2.3 |
 * +--------------+---------------------------------------------------------+------------------------------------------+---------------+
 */

#include <ostream>
#include <string>
#include <functional>

namespace RESTClient {
namespace http {
namespace decoder {

/// Used to get a line the tcp/ip connection
using GetLine = std::function<void(std::string&)>;

/// Used to copy n bytes from the tcp/ip connection to a string or stream (T)
template <typename T>
using GetN = std::function<void(size_t, T&)>;

/// Used to copy filter the content (for expample unzipping)
template <typename InIter, typename OutIter>
using Filter = std::function<void(InIter begin, InIter end, OutIter out)>;


size_t readChunk(GetLine getLine, GetN<std::string> getN, std::string& out);
void chunked(GetLine getLine, GetN<std::string> getN, std::string& out);
void chunked(GetLine getLine, GetN<std::ostream> getN, std::ostream& out);

void unzip(GetN<std::string> getN, std::string& out);
void zipped(GetN<std::ostream> getN, std::ostream& out);

} /* decoder  */ 
} /* http  */ 
} /* RESTClient  */ 
