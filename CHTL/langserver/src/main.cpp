#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../../../third-party/nlohmann/json.hpp"

using json = nlohmann::json;

void log_message(const std::string& message) {
    std::ofstream log_file("langserver.log", std::ios::app);
    log_file << message << std::endl;
}

int main() {
    log_message("Language server started.");

    while (std::cin) {
        std::string line;
        std::getline(std::cin, line);

        // LSP messages have a "Content-Length" header
        if (line.find("Content-Length: ") == 0) {
            int length = std::stoi(line.substr(16));
            std::cin.ignore(2); // Skip the \r\n

            std::vector<char> buffer(length);
            std::cin.read(buffer.data(), length);
            std::string content(buffer.begin(), buffer.end());

            log_message("Received: " + content);

            try {
                json request = json::parse(content);

                if (request.contains("method") && request["method"] == "initialize") {
                    json response = {
                        {"jsonrpc", "2.0"},
                        {"id", request["id"]},
                        {"result", {
                            {"capabilities", {
                                // 1 means "Full": the entire document is sent on every change.
                                {"textDocumentSync", 1}
                            }}
                        }}
                    };
                    std::string response_str = response.dump();
                    std::cout << "Content-Length: " << response_str.length() << "\r\n\r\n" << response_str;
                    std::cout.flush();
                    log_message("Sent Initialize Response: " + response_str);

                } else if (request.contains("method") && request["method"] == "initialized") {
                    // The "initialized" notification is sent from client to server after
                    // the client received the "initialize" response.
                    // No response is needed.
                    log_message("Received initialized notification.");

                } else if (request.contains("id")) {
                    // For any other request that has an ID, send a generic null response
                    // to prevent the client from timing out.
                    json response = {
                        {"jsonrpc", "2.0"},
                        {"id", request["id"]},
                        {"result", nullptr}
                    };
                    std::string response_str = response.dump();
                    std::cout << "Content-Length: " << response_str.length() << "\r\n\r\n" << response_str;
                    std::cout.flush();
                    log_message("Sent Generic Null Response: " + response_str);
                }

            } catch (const json::parse_error& e) {
                log_message("JSON parse error: " + std::string(e.what()));
            }
        }
    }

    log_message("Language server stopped.");
    return 0;
}
