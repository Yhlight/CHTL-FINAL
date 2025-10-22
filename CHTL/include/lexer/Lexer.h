#pragma once

#include "lexer/Token.h"
#include <string>

namespace CHTL
{
    /**
     * @class Lexer
     * @brief The Lexer class is responsible for taking a raw source string and breaking it
     *        down into a series of tokens.
     *
     * It scans the input character by character, groups them into meaningful tokens
     * (like identifiers, keywords, numbers, and operators), and provides them
     * to the Parser.
     */
    class Lexer
    {
    public:
        /**
         * @brief Constructs a Lexer with a given source string.
         * @param input The CHTL source code to be tokenized.
         */
        Lexer(const std::string& input);

        /**
         * @brief Scans the input and returns the next token.
         * @return The next token found in the source string.
         */
        Token NextToken();

        /**
         * @brief Reads a block of raw, un-tokenized content, typically used for
         *        things like `[Origin]` blocks.
         * @return A string containing the raw content of the block.
         */
        std::string readRawBlockContent();

        // Getters for parser to query state
        size_t GetPosition() const { return m_position; }
        size_t GetReadPosition() const { return m_readPosition; }
        const std::string& GetInput() const { return m_input; } // Needed for raw content parsing

    private:
        void readChar();
        char peekChar() const;
        void skipWhitespace();
        void skipSingleLineComment();
        void skipMultiLineComment();
        std::string readIdentifier();
        std::string readString(char quote_type);
        std::string readNumber();
        std::string readComment();
        Token readBlockKeyword();
        static bool isLetter(char ch);

        std::string m_input;         ///< The source code being tokenized.
        size_t m_position = 0;       ///< The current position in the input (points to the current character).
        size_t m_readPosition = 0;   ///< The next position to be read in the input.
        char m_char = 0;             ///< The current character being examined.
        int m_line = 1;              ///< The current line number for error reporting.
        int m_column = 0;            ///< The current column number for error reporting.
    };

} // namespace CHTL
