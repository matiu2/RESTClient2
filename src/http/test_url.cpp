#include "url.hpp"

#include <cassert>
#include <iostream>
#include <functional>
#include <map>
#include <algorithm>
#include <vector>
#include <regex>

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


int main(int argc, char** argv) {

  // Get the command line arguments
  std::vector<std::regex> args;
  for (int i = 1; i != argc; ++i) {
    std::regex regex(argv[i]);
    args.emplace_back(std::move(regex));
  }

  int returnVal = 0;

  auto check = [&returnVal](const char *name, const auto &val,
                            auto &&expected) {
    cout << RESET;
    if (val != expected) {
      cout << BOLDRED << "ERROR: ";
      ++returnVal;
    }
    cout << name << ": " << val << " - expected: " << expected << '\n';
  };

  std::map<std::string, std::function<void(URL &&, const std::string &)>> tests{
      {"http://something.com",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "http://something.com");
         check("is_ssl", url.is_ssl(), false);
         check("Protocol", url.protocol, "http");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "something.com");
         check("Path", url.path, "");
         check("Port", url.port, 80);
       }},
      {"https://more.secure.com/",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/");
         check("Port", url.port, 443);
       }},
      {"https://more.secure.com:8011/",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com:8011");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/");
         check("Port", url.port, 8011);
       }},
      {"https://more.secure.com:8011/some/path/to/somewhere",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com:8011");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/some/path/to/somewhere");
         check("Port", url.port, 8011);
       }},
      {"https://more.secure.com:8011/some/path/to/somewhere?a=b&b=c",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com:8011");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/some/path/to/somewhere");
         check("Port", url.port, 8011);
         check("Param Length", url.params.size(), 2);
         check("Param a", url.params["a"], "b");
         check("Param b", url.params["b"], "c");

       }},
      {"https://more.secure.com:8011?a=b&b=c",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com:8011");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "");
         check("Port", url.port, 8011);
         check("Param Length", url.params.size(), 2);
         check("Param a", url.params["a"], "b");
         check("Param b", url.params["b"], "c");
       }},
      {"https://more.secure.com?a=b&b=c",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "");
         check("Port", url.port, 443);
         check("Param Length", url.params.size(), 2);
         check("Param a", url.params["a"], "b");
         check("Param b", url.params["b"], "c");
       }},
      {"https://more.secure.com/has/a/path/?a=b&b=c",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/has/a/path/");
         check("Port", url.port, 443);
         check("Param Length", url.params.size(), 2);
         check("Param a", url.params["a"], "b");
         check("Param b", url.params["b"], "c");
       }},
      {"https://more.secure.com/has/a/path/?apple=oranges",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/has/a/path/");
         check("Port", url.port, 443);
         check("Param Length", url.params.size(), 1);
         check("Param apple", url.params["apple"], "oranges");
       }},
      {"https://username@more.secure.com/has/a/path/?apple=oranges",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "username");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/has/a/path/");
         check("Port", url.port, 443);
         check("Param Length", url.params.size(), 1);
         check("Param apple", url.params["apple"], "oranges");
       }},
      {"https://username:password@more.secure.com:2020/has/a/path/"
       "?apple=oranges",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com:2020");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "username");
         check("Password", url.password, "password");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/has/a/path/");
         check("Port", url.port, 2020);
         check("Param Length", url.params.size(), 1);
         check("Param apple", url.params["apple"], "oranges");
       }},
      {"https://username@more.secure.com:2020/?apple=oranges",
       [&](URL&& url, const std::string &str) {
         check("Parsed url", url.whole(), str);
         check("Host Part", url.host_part(), "https://more.secure.com:2020");
         check("is_ssl", url.is_ssl(), true);
         check("Protocol", url.protocol, "https");
         check("Username", url.username, "username");
         check("Password", url.password, "");
         check("Hostname", url.hostname, "more.secure.com");
         check("Path", url.path, "/");
         check("Port", url.port, 2020);
         check("Param Length", url.params.size(), 1);
         check("Param apple", url.params["apple"], "oranges");
       }},
  };

  for (const auto &pair : tests) {
    bool run = true;
    if (!args.empty()) {
      run = false;
      for (const auto &arg : args) {
        if (std::regex_search(pair.first, arg)) {
          run = true;
          break;
        }
      }
    }
    if (run) {
      cout << '\n'
           << "================================================================"
              "================"
           << '\n';
      cout << pair.first << '\n';
      pair.second(pair.first, pair.first);
    }
  }

  return returnVal;

}
