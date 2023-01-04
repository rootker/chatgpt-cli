#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;

//ADD THIS FUNCTION: NOTE the Void pointer "buffer"

size_t write_to_string(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t total_size = size * nmemb;
    std::string *response = (std::string *) stream;
    response->append((char *) ptr, total_size);
    return total_size;
}
int main() {
  // Initialize CURL library
	std::string ApiKey = "YOUR_API_KEY";
	std::string header_string = "Authorization: Bearer " + ApiKey;
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Error initializing CURL library" << std::endl;
    return 1;
  }

  // Set up the request
  curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/engines/text-davinci-003/completions"); 
  curl_easy_setopt(curl, CURLOPT_POST, 1);

  // Prompt the user for a question
  std::cout << "Enter a question: ";
  std::string question;
  std::getline(std::cin, question);
  //std::cout << "Your Question : " << question << std::endl;
  // Set the request body to the user's question
  std::string request_body = "{\"prompt\": [\"" + question + "\"]}";
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());

  // Set the headers
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  headers = curl_slist_append(headers, header_string.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // Set the callback function to handle the response
  std::string response;

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  // Send the request and receive the response
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "Error making request: " << curl_easy_strerror(res) << std::endl;
    return 1;
  }

  // Parse the response
  nlohmann::json response_json = nlohmann::json::parse(response);
  std::string answer = response_json["choices"][0]["text"];
  // Print the answer
  std::cout << "Answer: " << answer << "\n";
  // Clean up
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return 0;
}
