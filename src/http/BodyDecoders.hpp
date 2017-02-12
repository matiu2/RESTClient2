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

#include "../tcpip/SpyGuard.hpp"

namespace RESTClient {
namespace http {
namespace decoder {

using GetSpyGuard = std::function<tcpip::SpyGuard()>;

void chunked(GetSpyGuard getSpyGuard, std::string& out);
void chunked(GetSpyGuard getSpyGuard, std::ostream& out);

} /* decoder  */ 
} /* http  */ 
} /* RESTClient  */ 
