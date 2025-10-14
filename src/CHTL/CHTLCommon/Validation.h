#pragma once

#include "CompilerConfig.h"
#include "CompilerError.h"
#include <string>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <cctype>

namespace CHTL {

// 输入验证工具类
class Validation {
public:
    // 验证文件路径
    static void validateFilePath(const std::string& path) {
        if (path.empty()) {
            throw FileError("文件路径为空", path);
        }

        // 检查路径长度
        if (path.length() > 4096) {
            throw FileError("文件路径过长（超过 4096 字符）", path);
        }

        // 规范化路径
        std::filesystem::path fsPath(path);
        
        try {
            // 检查路径是否包含危险模式
            auto canonical = std::filesystem::weakly_canonical(fsPath);
            std::string canonicalStr = canonical.string();
            
            // 检查路径遍历
            if (canonicalStr.find("..") != std::string::npos) {
                throw FileError("检测到路径遍历尝试", path);
            }
        } catch (const std::filesystem::filesystem_error& e) {
            throw FileError(std::string("路径验证失败: ") + e.what(), path);
        }

        // 检查文件是否存在
        if (!std::filesystem::exists(fsPath)) {
            throw FileError("文件不存在", path);
        }

        // 检查是否是常规文件
        if (!std::filesystem::is_regular_file(fsPath)) {
            throw FileError("不是常规文件（可能是目录或特殊文件）", path);
        }

        // 检查文件大小
        auto fileSize = std::filesystem::file_size(fsPath);
        if (fileSize > CompilerConfig::MAX_FILE_SIZE) {
            throw FileError(
                "文件过大（" + std::to_string(fileSize) + " 字节，最大 " + 
                std::to_string(CompilerConfig::MAX_FILE_SIZE) + " 字节）",
                path
            );
        }

        // 检查文件是否可读
        std::ifstream file(path);
        if (!file.good()) {
            throw FileError("无法读取文件", path);
        }
    }

    // 验证字符串长度
    static void validateStringLength(const std::string& str, size_t maxLength, const std::string& name = "字符串") {
        if (str.length() > maxLength) {
            throw ResourceLimitError(
                name + "过长（" + std::to_string(str.length()) + " 字符，最大 " + 
                std::to_string(maxLength) + " 字符）"
            );
        }
    }

    // 验证嵌套深度
    static void validateNestingDepth(size_t depth) {
        if (depth > CompilerConfig::MAX_NESTING_DEPTH) {
            throw ResourceLimitError(
                "嵌套深度超限（" + std::to_string(depth) + "，最大 " + 
                std::to_string(CompilerConfig::MAX_NESTING_DEPTH) + "）"
            );
        }
    }

    // 验证 Token 数量
    static void validateTokenCount(size_t count) {
        if (count > CompilerConfig::MAX_TOKEN_COUNT) {
            throw ResourceLimitError(
                "Token 数量超限（" + std::to_string(count) + "，最大 " + 
                std::to_string(CompilerConfig::MAX_TOKEN_COUNT) + "）"
            );
        }
    }

    // 验证属性数量
    static void validateAttributeCount(size_t count) {
        if (count > CompilerConfig::MAX_ATTRIBUTES) {
            throw ResourceLimitError(
                "属性数量超限（" + std::to_string(count) + "，最大 " + 
                std::to_string(CompilerConfig::MAX_ATTRIBUTES) + "）"
            );
        }
    }

    // 验证子节点数量
    static void validateChildrenCount(size_t count) {
        if (count > CompilerConfig::MAX_CHILDREN) {
            throw ResourceLimitError(
                "子节点数量超限（" + std::to_string(count) + "，最大 " + 
                std::to_string(CompilerConfig::MAX_CHILDREN) + "）"
            );
        }
    }

    // 验证输出大小
    static void validateOutputSize(size_t size) {
        if (size > CompilerConfig::MAX_OUTPUT_SIZE) {
            throw ResourceLimitError(
                "输出大小超限（" + std::to_string(size) + " 字节，最大 " + 
                std::to_string(CompilerConfig::MAX_OUTPUT_SIZE) + " 字节）"
            );
        }
    }

    // 验证标识符
    static void validateIdentifier(const std::string& identifier) {
        if (identifier.empty()) {
            throw CompilerError(ErrorSeverity::ERROR, "标识符为空");
        }

        // 检查首字符
        if (!std::isalpha(identifier[0]) && identifier[0] != '_') {
            throw CompilerError(
                ErrorSeverity::ERROR, 
                "标识符必须以字母或下划线开头: " + identifier
            );
        }

        // 检查其他字符
        for (char c : identifier) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                throw CompilerError(
                    ErrorSeverity::ERROR,
                    "标识符包含非法字符: " + identifier
                );
            }
        }

        // 长度限制
        validateStringLength(identifier, 255, "标识符");
    }
};

} // namespace CHTL
