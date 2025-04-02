#include"Gemini.h"
#include <curl/curl.h>
#include <string>
#include <cstdlib>
#include <iostream>


gemini::gemini(std::string Prompt) : Prompt(Prompt){}

std::string gemini::generateResponse()
{
    const char* api_key = std::getenv("GEMINI_API_KEY");
    const char* URL = std::getenv("URL");
    if (!api_key) {
       std::cerr << "Error: API key not found. Please set the GEMINI_API_KEY environment variable." << std::endl;
       return ""; // Exit the program early
    }
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string data = R"({"contents": [{"parts":[{"text": ")"+Prompt+R"("}]}]})";
    std::string response_data;

    curl = curl_easy_init();
    if(curl){

        struct curl_slist* headers = NULL;
        
        // Add headers to the list
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string finalURL = std::string(URL)+std::string(api_key);
        


        curl_easy_setopt(curl, CURLOPT_URL, finalURL.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L); // Use POST request
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        return "";
        }
           
        curl_slist_free_all(headers);

        curl_easy_cleanup(curl);

    }
    // Parse JSON response and extract text
    Json::CharReaderBuilder reader;
    Json::Value jsonResponse;
    std::string errs;
    std::istringstream s(response_data);
    
    if (Json::parseFromStream(reader, s, &jsonResponse, &errs)) {
        if (jsonResponse.isMember("candidates") && jsonResponse["candidates"].isArray()) {
            return jsonResponse["candidates"][0]["content"]["parts"][0]["text"].asString();
        }
    }
    
    return "";
}

size_t gemini::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
