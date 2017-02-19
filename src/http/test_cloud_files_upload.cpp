/** Tests chunked uploading to cloud files **/ 
#include "interface.hpp"

#include <jsonpp11/parse_to_json_class.hpp>
#include <jsonpp11/json_class.hpp>
#include <cassert>
#include <iostream>
#include <fstream>
#include <set>

struct Config {
  std::string username;
  std::string apikey;
  std::string filenameToUpload;
};

auto upload = [](RESTClient::http::yield_context yield) {
  // Load the config file
  std::ifstream configFile(".rackspaceConfig");
  Config config;
  const std::map<std::string, std::function<void(const std::string &)>>
      configActions{
          {"username",
           [&config](const std::string &val) { config.username = val; }},
          {"apikey",
           [&config](const std::string &val) { config.apikey = val; }},
          {"filename", [&config](const std::string &val) {
             config.filenameToUpload = val;
           }}};
  while (!configFile.eof()) {
    std::string key, val;
    configFile >> key >> val;
    auto found = configActions.find(key);
    if (found != configActions.end())
      found->second(val);
    else
      std::cerr << "Unknown config entry: " << key << " " << val;
  }
  // Log in to rackspace
  using json::JMap;
  json::JSON authRequest(
      JMap{{"auth", JMap{JMap{{"RAX-KSKEY:apiKeyCredentials",
                               JMap{{"username", config.username},
                                    {"apiKey", config.apikey}}}}}}});
  auto resp = json::readValue(
      RESTClient::http::post("https://identity.api.rackspacecloud.com/v2.0/")
          .body(authRequest.toString())
          .go(yield)
          .body);
  std::string authToken = resp["access"]["token"]["id"];
  std::cout << "Auth token: " << authToken << std::endl;
};

int main(int, char**) {
  auto tmp = upload;
  RESTClient::http::spawn(tmp);
  RESTClient::http::run();
  return 0;
}
