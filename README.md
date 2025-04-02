# AI-Powered Ticket Processing System 

## Overview

This project is an AI-powered IT support ticketing system built using C++, designed to automate ticket processing by integrating with the Gemini AI API. It efficiently generates structured responses, including a title, summary, and troubleshooting steps for each issue. The system leverages multithreading for fast execution and JSON handling for seamless data processing.

Features

- AI-Driven Ticket Processing – Automatically generates responses using AI.
- Multithreading for Performance – Processes multiple tickets simultaneously.
- Used CURL to make HTTP web Requests.
- Secure API Handling – Uses environment variables for API credentials.
- JSON Input/Output – Reads support tickets from tickets.json and saves AI-generated responses to output_tickets.json.
- Scalable & Extendable – Can be integrated into existing IT support systems.

Technologies Used

- C++ (Modern C++ 20)
- JsonCpp Library – For handling JSON data
- Multithreading (std::thread, std::mutex)
- Google Gemini AI API
- File I/O Handling

Project Demo

- Example Input Ticket (tickets.json):

```
[
  {
    "CustomerName": "John Doe",
    "Issue Description": "Cannot connect to VPN",
    "TimeStamp": "2024-04-02T12:34:56Z"
  }
]
```

- Example AI-Generated Output (output_tickets.json):
```
[
  {
    "TicketID": "IS1",
    "Title": "VPN Connection Issue",
    "Summary": "User unable to connect to VPN. Possible network issue.",
    "Troubleshooting_Steps": [
      "Check internet connection.",
      "Restart the VPN client.",
      "Verify VPN credentials."
    ]
  }
]
```

## Future Enhancements 

- Web Interface – A dashboard to view and manage tickets.
- Database Integration – Store and retrieve ticket data in a database.
- Email Notifications – Notify customers of resolved issues.
- More AI Enhancements – Improve response accuracy with context-aware troubleshooting.