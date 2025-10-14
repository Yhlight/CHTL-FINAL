#pragma once

#include <stdexcept>
#include <string>
#include <sstream>

namespace CHTL {

// 编译器错误类型
enum class ErrorSeverity {
    WARNING,    // 警告
    ERROR,      // 错误
    FATAL       // 致命错误
};

// 详细的编译器错误类
class CompilerError : public std::runtime_error {
private:
    ErrorSeverity severity_;
    size_t line_;
    size_t column_;
    std::string filename_;
    std::string context_;
    std::string errorCode_;

public:
    CompilerError(
        ErrorSeverity severity,
        const std::string& message,
        const std::string& filename = "",
        size_t line = 0,
        size_t column = 0,
        const std::string& context = "",
        const std::string& errorCode = ""
    ) : std::runtime_error(formatMessage(severity, message, filename, line, column, context, errorCode))
      , severity_(severity)
      , line_(line)
      , column_(column)
      , filename_(filename)
      , context_(context)
      , errorCode_(errorCode)
    {}

    ErrorSeverity getSeverity() const { return severity_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    const std::string& getFilename() const { return filename_; }
    const std::string& getContext() const { return context_; }
    const std::string& getErrorCode() const { return errorCode_; }

    // 获取严重程度字符串
    static std::string severityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::WARNING: return "警告";
            case ErrorSeverity::ERROR:   return "错误";
            case ErrorSeverity::FATAL:   return "致命错误";
            default:                     return "未知";
        }
    }

private:
    static std::string formatMessage(
        ErrorSeverity severity,
        const std::string& message,
        const std::string& filename,
        size_t line,
        size_t column,
        const std::string& context,
        const std::string& errorCode
    ) {
        std::ostringstream oss;
        
        // 格式: [错误] filename:line:column: message
        oss << "[" << severityToString(severity) << "]";
        
        if (!filename.empty()) {
            oss << " " << filename;
            if (line > 0) {
                oss << ":" << line;
                if (column > 0) {
                    oss << ":" << column;
                }
            }
            oss << ":";
        }
        
        oss << " " << message;
        
        if (!errorCode.empty()) {
            oss << " [" << errorCode << "]";
        }
        
        if (!context.empty()) {
            oss << "\n  上下文: " << context;
        }
        
        return oss.str();
    }
};

// 具体的错误类型
class LexerError : public CompilerError {
public:
    LexerError(const std::string& message, size_t line, size_t column, const std::string& context = "")
        : CompilerError(ErrorSeverity::ERROR, message, "", line, column, context, "LEX") {}
};

class ParseError : public CompilerError {
public:
    ParseError(const std::string& message, size_t line, size_t column, const std::string& context = "")
        : CompilerError(ErrorSeverity::ERROR, message, "", line, column, context, "PARSE") {}
};

class GeneratorError : public CompilerError {
public:
    GeneratorError(const std::string& message, const std::string& context = "")
        : CompilerError(ErrorSeverity::ERROR, message, "", 0, 0, context, "GEN") {}
};

class FileError : public CompilerError {
public:
    FileError(const std::string& message, const std::string& filename)
        : CompilerError(ErrorSeverity::FATAL, message, filename, 0, 0, "", "FILE") {}
};

class ResourceLimitError : public CompilerError {
public:
    ResourceLimitError(const std::string& message, const std::string& context = "")
        : CompilerError(ErrorSeverity::FATAL, message, "", 0, 0, context, "LIMIT") {}
};

} // namespace CHTL
