#include "url.hpp"

#include <cassert>
#include <iostream>

using namespace std;
using namespace RESTClient::http;

// The following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

int main(int, char**) {

  std::string str("http://something.com");
  URL url(str);

  int returnVal = 0;

  auto check = [&returnVal](const char *name, const auto &val,
                            auto &&expected) {
    cout << RESET;
    if (val != expected) {
      cout << BOLDRED;
      ++returnVal;
    }
    cout << name << ": " << val << " - expected: " << expected << '\n';
  };

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "http");
  check("Hostname", url.hostname, "something.com");
  check("Path", url.path, "");
  check("Port", url.port, 80);

  cout << '\n' << "================================================================================" << '\n';

  // New URL
  str = "https://more.secure.com/";
  url = str;

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "https");
  check("Hostname", url.hostname, "more.secure.com");
  check("Path", url.path, "/");
  check("Port", url.port, 443);

  cout << '\n' << "================================================================================" << '\n';

  // Custom port
  str = "https://more.secure.com:8011/";
  url = str;

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "https");
  check("Hostname", url.hostname, "more.secure.com");
  check("Path", url.path, "/");
  check("Port", url.port, 8011);

  cout << '\n' << "================================================================================" << '\n';

  // Add a path
  str = "https://more.secure.com:8011/some/path/to/somewhere";
  url = str;

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "https");
  check("Hostname", url.hostname, "more.secure.com");
  check("Path", url.path, "/some/path/to/somewhere");
  check("Port", url.port, 8011);

  cout << '\n' << "================================================================================" << '\n';

  // Add some params
  str = "https://more.secure.com:8011/some/path/to/somewhere?a=b&b=c";
  url = str;

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "https");
  check("Hostname", url.hostname, "more.secure.com");
  check("Path", url.path, "/some/path/to/somewhere");
  check("Port", url.port, 8011);
  check("Param Length", url.params.size(), 2);
  check("Param a", url.params["a"], "b");
  check("Param b", url.params["b"], "c");

  cout << '\n' << "================================================================================" << '\n';

  // Now take away the path
  str = "https://more.secure.com:8011?a=b&b=c";
  url = str;

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "https");
  check("Hostname", url.hostname, "more.secure.com");
  check("Path", url.path, "");
  check("Port", url.port, 8011);
  check("Param Length", url.params.size(), 2);
  check("Param a", url.params["a"], "b");
  check("Param b", url.params["b"], "c");

  cout << '\n' << "================================================================================" << '\n';

  // Now take away the port
  str = "https://more.secure.com?a=b&b=c";
  url = str;

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "https");
  check("Hostname", url.hostname, "more.secure.com");
  check("Path", url.path, "");
  check("Port", url.port, 443);
  check("Param Length", url.params.size(), 2);
  check("Param a", url.params["a"], "b");
  check("Param b", url.params["b"], "c");
  for (const auto& pair : url.params) {
    cout << pair.first << " = " << pair.second << "\n";
  }

  cout << '\n' << "================================================================================" << '\n';

  // Now put the path back
  str = "https://more.secure.com/has/a/path/?a=b&b=c";
  url = str;

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "https");
  check("Hostname", url.hostname, "more.secure.com");
  check("Path", url.path, "/has/a/path/");
  check("Port", url.port, 443);
  check("Param Length", url.params.size(), 2);
  check("Param a", url.params["a"], "b");
  check("Param b", url.params["b"], "c");

  cout << '\n' << "================================================================================" << '\n';

  // Try with a single param
  str = "https://more.secure.com/has/a/path/?apple=oranges";
  url = str;

  check("Parsed url", url.original(), str);
  check("Protocol", url.protocol, "https");
  check("Hostname", url.hostname, "more.secure.com");
  check("Path", url.path, "/has/a/path/");
  check("Port", url.port, 443);
  check("Param Length", url.params.size(), 1);
  check("Param apple", url.params["apple"], "oranges");

  return returnVal;
}
