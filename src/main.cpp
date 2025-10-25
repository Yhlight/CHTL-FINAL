#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    std::string content;
    bool in_text_block = false;
    while (std::getline(file, line)) {
        if (line.find("text") != std::string::npos) {
            in_text_block = true;
            continue;
        }
        if (in_text_block) {
            if (line.find("{") != std::string::npos) {
                continue;
            }
            if (line.find("}") != std::string::npos) {
                break;
            }
            size_t start = line.find_first_of("\"");
            size_t end = line.find_last_of("\"");
            if (start != std::string::npos && end != std::string::npos && start != end) {
                content = line.substr(start + 1, end - start - 1);
            }
        }
    }

    std::cout << content << std::endl;

    return 0;
}
