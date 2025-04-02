#include <curl/curl.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <json/json.h>

class gemini
{
    std::string Prompt;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    public:
    gemini(std::string Prompt);
    std::string generateResponse();
   

};