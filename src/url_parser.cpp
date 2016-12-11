#include "url_parser.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/tuple.hpp>
#include <sstream>
#include <stdexcept>
#include <cassert>

namespace x3 = boost::spirit::x3;


namespace parser {

using x3::char_;
using x3::lit;
using x3::eol;
using x3::eoi;
using x3::space;

auto protocol = (x3::string("https") | x3::string("http")) >> "://";

// Everything auth related
auto username = +(x3::char_ - (char_(":") | '@' | eoi | eol));
auto password = lit(':') >> +(x3::char_ - '@');
auto userpass = username >> (password | '@');

auto hostname = +(char_ - (lit('?') | "/" | x3::eol | x3::eoi));
auto port = lit(':') >> x3::ushort_;
auto path = char_('/') >> *(char_ - (lit('?') | eol | eoi));

// params
auto param = +(char_ - '=');
auto value = +(char_ - ('&' | eoi | eol));
auto param_pair = param >> '=' >> value;
auto params = lit('?') >> param_pair % '&';

auto url = protocol >> -userpass >> hostname >> -port >> -path; // minus params

}

namespace RESTClient
{
  

URL::URL(const std::string &url) {
  auto i = url.begin();
  auto end = url.end();

  // Read the protocol
  bool ok =
      x3::phrase_parse(i, end, parser::protocol, parser::space, protocol);
  if (!ok) {
    std::stringstream msg;
    msg << "No protocol found in '" << url
        << "'. It should start with: 'http://' or 'https://'";
    throw std::runtime_error(msg.str());
  }

  // Read the username and password
  auto userpass = boost::fusion::tie(username, password);
  ok = x3::phrase_parse(i, end, parser::userpass, parser::space, userpass);
  if (!ok) {
    username = "";
    password = "";
  }
  
  // Read the hostname
  ok = x3::phrase_parse(i, end, parser::hostname, parser::space, hostname);
  if (!ok) {
    std::stringstream msg;
    msg << "Couldn't parse a hostname from this URL: " << url;
    throw std::runtime_error(msg.str());
  }

  // Read the port
  ok = x3::phrase_parse(i, end, parser::port, parser::space, port);
  if (!ok) {
    if (protocol == "http")
      port = 80;
    else {
      assert(protocol == "https");
      port = 443;
    }
  }

  // Read the path
  x3::phrase_parse(i, end, parser::path, parser::space, path);

  // Read the params
  while (i != end) {
    std::string param, value;
    ok = x3::phrase_parse(i, end, parser::param, parser::space, param);
    if (!ok) {
      std::stringstream msg;
      msg << "Expected a 'param=' but got no equals at position : "
          << i - url.begin() << " in url: " << url;
      throw std::runtime_error(msg.str());
    }
    ok = x3::phrase_parse(i, end, parser::value, parser::space, value);
    if (!ok) {
      std::stringstream msg;
      msg << "Expected a param value, followed by an '&' or end of line or end "
             "of input, but got somethnig else at position : "
          << i - url.begin() << " in url: " << url;
      throw std::runtime_error(msg.str());
    }
  }
}

std::string URL::url() const {
  std::stringstream out;
  out << protocol << "://";
  if (!username.empty()) {
    out << username;
    if (!password.empty())
      out << ":" << password;
    out << '@';
  }
  out << hostname;
  if (!(((protocol == "http") && (port == 80)) ||
        ((protocol == "https") && (port == 443)))) {
    out << ':' << port;
  }
  if (!path.empty())
    out << path;
  if (params.size() > 0) {
    out << '?';
    auto end = --params.end();
    for (auto i = params.begin(); i != end; ++i)
      out << i->first << '=' << i->second << '&';
    out << end->first << '=' << end->second << '&';
  }
  return out.str();
}

} /* RESTClient */ 
