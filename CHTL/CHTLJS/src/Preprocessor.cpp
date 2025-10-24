#include "Preprocessor.h"
#include <regex>

namespace CHTLJS {

std::string Preprocessor::process(const std::string& input) {
    // This is a simplified implementation. A real implementation would need a more robust way to identify CHTL JS blocks.
    // For now, we'll assume that any block of code that is not inside a string literal and contains CHTL JS keywords is a CHTL JS block.

    std::string processed_input = input;

    // For the sake of this example, let's assume CHTL JS blocks are wrapped in a special comment.
    // In a real scenario, we would use a more sophisticated method to identify these blocks.
    std::regex chtljs_block_regex(R"(ScriptMain\s*\{[\s\S]*?\}|\{\{[\s\S]*?\}\}->Listen\s*\{[\s\S]*?\}|\{\{[\s\S]*?\}\}->Delegate\s*\{[\s\S]*?\}|Animate\s*\{[\s\S]*?\}|Vir\s+[\s\S]*?=|Router\s*\{[\s\S]*?\}|\{\{[\s\S]*?\}\}->width|\{\{[\s\S]*?\}\}->height|\$[a-zA-Z_][a-zA-Z0-9_]*\$|\{\{[\s\S]*?\}\})");

    std::string result;
    auto words_begin = std::sregex_iterator(processed_input.begin(), processed_input.end(), chtljs_block_regex);
    auto words_end = std::sregex_iterator();

    long last_pos = 0;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        result += processed_input.substr(last_pos, match.position() - last_pos);
        result += "[__CHTLJS__]" + match.str() + "[__ENDCHTLJS__]";
        last_pos = match.position() + match.length();
    }
    result += processed_input.substr(last_pos);

    return result;
}

} // namespace CHTLJS
