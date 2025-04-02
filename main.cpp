#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include "Gemini.h"  
#include <json/json.h>  
#include <regex>

std::mutex mtx;  
int ticketCounter = 1;

struct Ticket {
    std::string customerName;
    std::string issueDescription;
    std::string timestamp;
};

struct Response {
    std::string ticketID;
    std::string title;
    std::string summary;
    std::vector<std::string> troubleshootingSteps; 

};

std::string cleanAIResponse(const std::string& response) {
    std::string cleaned = response;

    cleaned = std::regex_replace(cleaned, std::regex("^```json\\s*|\\s*```$"), "");

    cleaned = std::regex_replace(cleaned, std::regex("^\\s+|\\s+$"), "");

    return cleaned;
}

void processTicket(const Ticket& ticket, std::vector<Response>& responses) {
   
    mtx.lock();
    std::string ticketID = "IS" + std::to_string(ticketCounter++);
    mtx.unlock();

   
    std::string prompt = "Following is an Issue description:" + ticket.issueDescription +
                         "Generate Title, summary and Troubleshooting_Steps in JSON format. Keep the troubleshooting steps in single line only.";
    
  
    gemini gem(prompt);
    std::string aiResponse = gem.generateResponse(); 
    std::cout << "AI Response for Ticket " << ticketID << ":\n" << aiResponse << "\n";
    
  
    std::string cleanedResponse = cleanAIResponse(aiResponse);

    Json::Value aiJson;
    Json::Reader reader;
    if (!reader.parse(cleanedResponse, aiJson)) {
        std::cerr << "Error: Failed to parse AI response for ticket " << ticketID << "\n";
        std::cerr << "Raw Response: " << aiResponse << "\n";
        return;
    }
    
    std::string titleKey = aiJson.isMember("Title") ? "Title" : "title";
    std::string summaryKey = aiJson.isMember("Summary") ? "Summary" : "summary";
    std::string troubleshootingSteps = aiJson.isMember("troubleshooting_steps") ? "troubleshooting_steps" : "Troubleshooting_Steps";



    Response response;
    response.ticketID = ticketID;
    response.title = aiJson[titleKey].asString();
    response.summary = aiJson[summaryKey].asString();
    for (const auto& step : aiJson[troubleshootingSteps]) {
        response.troubleshootingSteps.push_back(step.asString());

    }
    
    Json::Value ticketJson;
    ticketJson["TicketID"] = response.ticketID;
    ticketJson[titleKey] = response.title;
    ticketJson[summaryKey] = response.summary;

    // Convert vector<string> to JSON array
    for (const std::string& step : response.troubleshootingSteps) {
         ticketJson[troubleshootingSteps].append(step);
    }

    std::cout << "Generated Ticket JSON:\n" << ticketJson.toStyledString() << std::endl;
    
    
    mtx.lock();
    responses.push_back(response);
    mtx.unlock();
}

int main() {
    std::ifstream file("tickets.json");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open tickets.json" << std::endl;
        return 1;
    }
    
    Json::Value inputJson;
    Json::Reader reader;
    if (!reader.parse(file, inputJson)) {
        std::cerr << "Error: Failed to parse JSON" << std::endl;
        return 1;
    }
    file.close();
    
    std::vector<Ticket> tickets;
    std::vector<Response> responses;
    std::vector<std::thread> threads;
    
   
    for (const auto& item : inputJson) {
        tickets.push_back({
            item["CustomerName"].asString(),
            item["Issue Description"].asString(),
            item["TimeStamp"].asString()
        });
    }
    
   
    for (const auto& ticket : tickets) {
        threads.emplace_back(processTicket, ticket, std::ref(responses));
    }
    

    for (auto& th : threads) {
        th.join();
    }
   
    
    Json::Value outputJson;
    for (const auto& res : responses) {
        Json::Value ticketJson;
        ticketJson["TicketID"] = res.ticketID;
        ticketJson["Title"] = res.title;
        ticketJson["Summary"] = res.summary;
        for (const auto& step : res.troubleshootingSteps) {
            ticketJson["Troubleshooting_Steps"].append(step);
        }
        
        outputJson.append(ticketJson);
    }
    
    std::ofstream outFile("output_tickets.json");
    Json::StreamWriterBuilder writer;
    outFile << Json::writeString(writer, outputJson);
    outFile.close();
    
    std::cout << "All tickets processed! Check output_tickets.json" << std::endl;
    return 0;
}
