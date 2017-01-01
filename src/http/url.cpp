#include "url.hpp"

#include <boost/spirit/home/x3.hpp>
#include <sstream>
#include <boost/range/iterator_range.hpp>


namespace url_parser {

using namespace boost::spirit::x3;

auto protocol = (string("https") | string("http")) >> "://";
auto hostname = +(char_ - (lit(':') | '?' | '/' | eoi));
auto port = lit(':') >> ushort_;
auto path = +(char_ - (lit('?') | eoi));
auto key =  raw[+(char_ - lit('='))];
auto val =  raw[+(char_ - (lit('?') - eoi))];
auto spacer = space;

} /* url_parser */ 

namespace RESTClient {
namespace http {

URL::URL(std::string url) {
  auto begin = url.cbegin();
  auto i = url.cbegin();
  auto end = url.cend();
  auto distance = [&]() { return std::distance(begin, i); };
  using boost::spirit::x3::phrase_parse;
  // Parse the protocol
  bool ok = phrase_parse(i, end, url_parser::protocol, url_parser::spacer,
                         this->protocol);
  if (!ok) {
    std::stringstream msg;
    msg << "Couldn't read protocol at position " << distance()
        << " in url: " << url;
    throw std::runtime_error(msg.str());
  }
  // Parse the hostname
  ok = phrase_parse(i, end, url_parser::hostname, url_parser::spacer, hostname);
  if (!ok) {
    std::stringstream msg;
    msg << "Couldn't read protocol at position " << distance()
        << " in url: " << url;
    throw std::runtime_error(msg.str());
  }
  // Parse the port
  ok = phrase_parse(i, end, url_parser::port, url_parser::spacer, port);
  if (!ok) {
    // If no explicit port is set, set it from the protocol
    port = protocol == "https" ? 443 : 80;
  }
  // Parse the path
  ok = phrase_parse(i, end, url_parser::path, url_parser::spacer, path);
  if (!ok) {
    // If no explicit path is set, set, assume '/'
    path = "/";
  }

  // See if there are any params
  ok = phrase_parse(i, end, url_parser::lit('?'), url_parser::spacer);
  if (ok) {
    while (i != end) {
      std::string key, val;
      // Get each key
      ok = phrase_parse(i, end, url_parser::key, url_parser::spacer, key);
      if (!ok) {
        std::stringstream msg;
        msg << "Unable to understand get paramater name at " << distance()
            << " for url: " << url;
        throw std::runtime_error(msg.str());
      }
      // The '=' symbol
      ok = phrase_parse(i, end, url_parser::lit('='), url_parser::spacer);
      if (!ok) {
        std::stringstream msg;
        msg << "Expected an '=' at " << distance()
            << " for url: " << url;
        throw std::runtime_error(msg.str());
      }
      // .. and each param
      ok = phrase_parse(i, end, url_parser::val, url_parser::spacer, val);
      if (!ok) {
        std::stringstream msg;
        msg << "Unable to understand get paramater value at " << distance()
            << " for url: " << url;
        throw std::runtime_error(msg.str());
      }
      // Now save them
      params.insert(std::make_pair(key, val));
    }
  }

  // Make sure we parsed it all
  if (i != end) {
    std::stringstream msg;
    msg << "Couldn't parse the entire URL string. Stopped at " << distance()
        << ". URL: " << url;
    throw std::runtime_error(msg.str());
  }
}

std::string URL::original() const {
  std::stringstream out;
  out << protocol << "://" << hostname;
  if (((protocol == "http") && (port != 80)) ||
      ((protocol == "https") && (port != 443))) {
    out << ':' << port;
  }
  out << path;
  if (!params.empty()) {
    out << '?';
    for (const auto &pair : params) {
      out << pair.first << '=' << pair.second;
    }
  }
  return out.str();
}

} /* http */
} /* RESTClient  */ 

