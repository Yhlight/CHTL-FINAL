#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>

// Heuristics to identify a line that likely starts a CHTL JS statement.
bool startsChtlJs(const std::string& line) {
    return line.find("{{") != std::string::npos ||
           line.find("->") != std::string::npos ||
           line.find("Listen {") != std::string::npos ||
           line.find("Delegate {") != std::string::npos ||
           line.find("Animate {") != std::string::npos ||
           line.find("Vir ") != std::string::npos;
}

// Function to count braces on a line
int countBraces(const std::string& line) {
    int count = 0;
    for (char c : line) {
        if (c == '{') {
            count++;
        } else if (c == '}') {
            count--;
        }
    }
    return count;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::ifstream inFile(argv[1]);
    if (!inFile) {
        std::cerr << "Error: Cannot open input file " << argv[1] << std::endl;
        return 1;
    }

    std::ofstream outFile(argv[2]);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file " << argv[2] << std::endl;
        return 1;
    }

    std::string line;
    bool in_script_block = false;
    bool in_chtljs_block = false;
    int script_brace_count = 0;
    int chtljs_brace_count = 0;

    while (getline(inFile, line)) {
        if (!in_script_block) {
            size_t script_pos = line.find("script");
            size_t brace_pos = line.find("{");
            // Enter script block if "script" is followed by "{"
            if (script_pos != std::string::npos && brace_pos != std::string::npos && brace_pos > script_pos) {
                in_script_block = true;
                script_brace_count = 0;
            }
            outFile << line << std::endl;
            if (in_script_block) {
                script_brace_count += countBraces(line);
            }
        } else { // We are in a script block
            if (!in_chtljs_block) {
                if (startsChtlJs(line)) {
                    outFile << "[CHTLJS]" << std::endl;
                    in_chtljs_block = true;
                    chtljs_brace_count = 0;
                }
            }

            outFile << line << std::endl;

            if (in_chtljs_block) {
                chtljs_brace_count += countBraces(line);
                // Heuristic for ending a CHTL JS statement: `};` on a line and balanced braces for the statement.
                if (line.find("};") != std::string::npos && chtljs_brace_count <= 0) {
                    outFile << "[CHTLJSEND]" << std::endl;
                    in_chtljs_block = false;
                }
            }

            script_brace_count += countBraces(line);
            if (script_brace_count <= 0) {
                if (in_chtljs_block) {
                    // Script block ended, but we are still in a CHTL JS block. Close it.
                    outFile << "[CHTLJSEND]" << std::endl;
                    in_chtljs_block = false;
                }
                in_script_block = false;
            }
        }
    }

    inFile.close();
    outFile.close();

    return 0;
}
