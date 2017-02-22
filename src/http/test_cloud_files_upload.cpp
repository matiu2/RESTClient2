/** Tests chunked uploading to cloud files **/ 
#include "interface.hpp"

#include <jsonpp11/parse_to_json_class.hpp>
#include <jsonpp11/json_class.hpp>
#include <iterator>
#include <fstream>
#include <cassert>
#include <iostream>
#include <set>


using namespace std::string_literals;

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
        std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()));
    username = config["username"];
    apikey = config["apikey"];
    filename = config["filename"];
    region = config["region"];
  }
};

using RESTClient::http::yield_context;

RESTClient::http::Headers baseHeaders{{"Accept", "application/json"},
                                      {"Content-Type", "application/json"}};
std::string authToken;
json::JMap authJSON;

void auth(yield_context &yield, const std::string &username,
          const std::string &apikey) {
  if (authToken.empty()) {
    using json::JMap;
    json::JSON authRequest(
        JMap{{"auth",
              JMap{JMap{{"RAX-KSKEY:apiKeyCredentials",
                         JMap{{"username", username}, {"apiKey", apikey}}}}}}});
    std::string resp =
        RESTClient::http::post(
            "https://identity.api.rackspacecloud.com/v2.0/tokens")
            .body(authRequest.toString())
            .add_header("Accept", "application/json")
            .add_header("Content-Type", "application/json")
            .go(yield)
            .body;
    try {
      authJSON = json::readValue(resp.begin(), resp.end());
    } catch (std::exception &e) {
      std::cerr << "Unable to read json response: " << resp << '\n';
      throw e;
    }
    try {
      authToken = authJSON["access"]["token"]["id"];
    } catch (std::exception &e) {
      std::cerr << "No token found in json: " << resp << '\n';
      throw e;
    }
    baseHeaders.insert(std::make_pair("X-Auth-Token", authToken));
  }
}

void upload(yield_context yield, const Config &config) {
  // Log in to rackspace
  try {
    auth(yield, config.username, config.apikey);
    std::cout << "Auth token: " << authToken << std::endl;
    // Now get the URL of the cloud files
    std::string url;
    const json::JList& serviceCatalog(authJSON["access"]["serviceCatalog"]);
    for (const json::JMap& page : serviceCatalog) {
      if (page.at("name") == "cloudFiles"s) {
        for (const json::JMap &entry :
             static_cast<const json::JList &>(page.at("endpoints")))
          if (entry.at("region") == config.region) {
            url = entry.at("publicURL");
            break;
          }
        break;
      }
    }
    std::cout << "Cloud files url for " << config.region << ": " << url
              << std::endl;
    const std::string& accountID = authJSON["access"]["token"]["tenant"]["id"];
    const std::string containerName = "test-restclient";
    
    // Check if the container is there
    auto resp = RESTClient::http::get(url + "/" + containerName)
                    .headers(baseHeaders)
                    .go(yield);

    if (resp.code == 404) {
      // Create the container (don't worry if it fails; just try)
      resp = RESTClient::http::put(url + "/" + containerName)
                 .headers(baseHeaders)
                 .go(yield);
      std::cout << "Created container: " << resp.code << " - " << resp.body
                << std::endl;
    } else {
      std::cout << "Container already exists: " << resp.code << std::endl;
    }

    // Upload a file
    std::ifstream file(config.filename);
    std::cout << "Uploading..." << std::endl;
    resp =
        RESTClient::http::put(url + "/" + containerName + "/" + config.filename)
            .headers(baseHeaders)
            .body(file)
            .go(yield);
    std::cout << "Upload result: " << resp.code << ": " << resp.body
              << std::endl;

    // Now download it
    std::cout << "Downloading..." << std::endl;
    std::ofstream down("downloaded.txt");
    resp =
        RESTClient::http::get(url + "/" + containerName + "/" + config.filename)
            .headers(baseHeaders)
            .saveToStream(down)
            .go(yield);
    std::cout << "Download result: " << resp.code << ": " << resp.body
              << std::endl;
    std::cout << "Downloading to downloaded.txt" << std::endl;
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
