/** Tests chunked uploading to cloud files **/ 
#include "interface.hpp"

#include <jsonpp11/parse_to_json_class.hpp>
#include <jsonpp11/json_class.hpp>
#include <iterator>
#include <fstream>
#include <cassert>
#include <iostream>
#include <set>


const std::string configFilename(".cloudUploadTest.json");

struct Config {
  // Load the config file
  std::string username;
  std::string apikey;
  std::string region;
  std::string filename;
  Config() {
    std::ifstream f(configFilename);
    if (!f.is_open()) {
      if (!f.good()) {
        std::stringstream msg;
        msg << "Couldn't load config file '" << configFilename
            << "'. It should be in json format: "
            << "{\n"
            << "    \"username\": \"my_user_name\",\n"
            << "    \"apikey\": \"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\",\n"
            << "    \"filename\": \"small.txt\",\n"
            << "   \"region\": \"SYD\"\n"
            << "}\n";
        throw std::runtime_error(msg.str());
      }
    }
    json::JMap config = json::readValue(std::string(
        std::istream_iterator<char>(f), std::istream_iterator<char>()));
    username = config["username"];
    apikey = config["apikey"];
    filename = config["filename"];
    region = config["region"];
  }
};

using RESTClient::http::yield_context;

std::string authToken;
json::JMap authJSON;

void auth(yield_context& yield, const std::string& username, const std::string& apikey) {
  if (authToken.empty()) {
    using json::JMap;
    json::JSON authRequest(
        JMap{{"auth", JMap{JMap{{"RAX-KSKEY:apiKeyCredentials",
                                 JMap{{"username", username},
                                      {"apiKey", apikey}}}}}}});
    std::string resp =
        RESTClient::http::post("https://identity.api.rackspacecloud.com/v2.0/tokens")
            .body(authRequest.toString())
            .add_header("Accept", "application/json")
            .add_header("Content-Type", "application/json")
            .go(yield)
            .body;
    try {
      authJSON = json::readValue(resp.begin(), resp.end());
    } catch(std::exception& e) {
      std::cerr << "Unable to read json response: " << resp << '\n';
      throw e;
    }
    try {
      authToken = authJSON["access"]["token"]["id"];
    } catch(std::exception& e) {
      std::cerr << "No token found in json: " << resp << '\n';
      throw e;
    }
  }
}

void upload(yield_context yield, const Config &config) {
  // Log in to rackspace
  try {
    auth(yield, config.username, config.apikey);
    std::cout << "Auth token: " << authToken << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception" << std::endl;
  }
};

int main(int, char**) {
  Config config;
  RESTClient::http::spawn([&config](yield_context y) { upload(y, config); });
  RESTClient::http::run();
  return 0;
}
