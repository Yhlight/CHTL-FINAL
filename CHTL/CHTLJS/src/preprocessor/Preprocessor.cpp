#include "Preprocessor.h"
#include <vector>
#include <sstream>

namespace CHTLJS
{
    std::string Preprocessor::Process(const std::string& input)
    {
        std::stringstream output;
        std::vector<std::string> chtmljs_keywords = {
            "ScriptLoader", "Listen", "Delegate", "Animate", "Vir", "Router"
        };

        size_t current_pos = 0;
        while (current_pos < input.length())
        {
            size_t next_keyword_pos = std::string::npos;
            std::string found_keyword;

            // Find the earliest CHTL JS keyword
            for (const auto& keyword : chtmljs_keywords)
            {
                size_t pos = input.find(keyword, current_pos);
                if (pos != std::string::npos && (next_keyword_pos == std::string::npos || pos < next_keyword_pos))
                {
                    next_keyword_pos = pos;
                    found_keyword = keyword;
                }
            }

            size_t next_selector_pos = input.find("{{", current_pos);

            size_t start_pos = std::string::npos;
            bool is_selector = false;

            if (next_keyword_pos != std::string::npos && (next_selector_pos == std::string::npos || next_keyword_pos < next_selector_pos)) {
                start_pos = next_keyword_pos;
            } else if (next_selector_pos != std::string::npos) {
                start_pos = next_selector_pos;
                is_selector = true;
            }


            if (start_pos != std::string::npos)
            {
                // Append the JavaScript part before the CHTL JS block
                if (start_pos > current_pos)
                {
                    output << input.substr(current_pos, start_pos - current_pos);
                }

                // Find the end of the CHTL JS block
                size_t end_pos = std::string::npos;
                if (is_selector) {
                    end_pos = input.find("}}", start_pos);
                    if (end_pos != std::string::npos) {
                        end_pos += 2; // Include "}}"
                         // Look for a semicolon to terminate the statement
                        size_t semicolon_pos = input.find(";", end_pos);
                        if (semicolon_pos != std::string::npos) {
                            end_pos = semicolon_pos + 1;
                        }
                    }
                } else {
                    size_t brace_pos = input.find("{", start_pos);
                    if (brace_pos != std::string::npos) {
                        int brace_count = 1;
                        size_t search_pos = brace_pos + 1;
                        while (search_pos < input.length() && brace_count > 0) {
                            if (input[search_pos] == '{') {
                                brace_count++;
                            } else if (input[search_pos] == '}') {
                                brace_count--;
                            }
                            search_pos++;
                        }
                        if (brace_count == 0) {
                            end_pos = search_pos;
                             // Look for a semicolon to terminate the statement
                            size_t semicolon_pos = input.find(";", end_pos);
                            if (semicolon_pos != std::string::npos) {
                                end_pos = semicolon_pos + 1;
                            }
                        }
                    } else {
                        // Handle Vir case which ends with a semicolon
                        size_t semicolon_pos = input.find(";", start_pos);
                        if (semicolon_pos != std::string::npos) {
                            end_pos = semicolon_pos + 1;
                        }
                    }
                }

                if (end_pos != std::string::npos)
                {
                    output << " [__CHTLJS__] " << input.substr(start_pos, end_pos - start_pos) << " [__CHTLJSEND__] ";
                    current_pos = end_pos;
                }
                else
                {
                    // Could not find end of block, treat the rest as JS
                    output << input.substr(current_pos);
                    current_pos = input.length();
                }
            }
            else
            {
                // No more CHTL JS keywords, append the rest
                output << input.substr(current_pos);
                current_pos = input.length();
            }
        }
        return output.str();
    }

} // namespace CHTLJS
