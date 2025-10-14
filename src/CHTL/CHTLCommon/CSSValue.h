#pragma once

#include <string>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace CHTL {

/**
 * @brief CSS 值类 - 表示带单位的CSS值（如100px, 50%, 2em）
 * 
 * 严格遵循 CHTL.md 第175-202行规范：
 * - 支持算术运算 (+ - * / % **)
 * - 同单位合并
 * - 不同单位报错
 * - 左结合规则
 * - 百分号特殊处理
 */
class CSSValue {
public:
    // 构造函数
    CSSValue() : value_(0.0), unit_("") {}
    CSSValue(double value, const std::string& unit = "") 
        : value_(value), unit_(unit) {}
    
    // 访问器
    double getValue() const { return value_; }
    std::string getUnit() const { return unit_; }
    bool hasUnit() const { return !unit_.empty(); }
    
    // 算术运算符
    CSSValue operator+(const CSSValue& other) const;
    CSSValue operator-(const CSSValue& other) const;
    CSSValue operator*(const CSSValue& other) const;
    CSSValue operator/(const CSSValue& other) const;
    CSSValue operator%(const CSSValue& other) const;  // 取模
    
    // 幂运算
    CSSValue power(const CSSValue& other) const;
    
    // 一元运算符
    CSSValue operator-() const;
    CSSValue operator+() const;
    
    // 转换为字符串
    std::string toString() const;
    
    // 单位验证
    bool canMergeWith(const CSSValue& other, char op) const;
    
    // 静态工厂方法
    static CSSValue fromString(const std::string& str);
    
private:
    double value_;
    std::string unit_;
    
    // 格式化数值（去除不必要的小数）
    std::string formatValue(double val) const;
};

// 实现

inline CSSValue CSSValue::operator+(const CSSValue& other) const {
    // CHTL.md 第184-185行：同单位合并，不同单位报错
    if (!canMergeWith(other, '+')) {
        throw std::runtime_error("单位不匹配: 无法对 " + unit_ + " 和 " + 
                                 other.unit_ + " 进行加法运算");
    }
    
    // 左结合：如果other无单位，继承this的单位
    std::string resultUnit = hasUnit() ? unit_ : other.unit_;
    return CSSValue(value_ + other.value_, resultUnit);
}

inline CSSValue CSSValue::operator-(const CSSValue& other) const {
    if (!canMergeWith(other, '-')) {
        throw std::runtime_error("单位不匹配: 无法对 " + unit_ + " 和 " + 
                                 other.unit_ + " 进行减法运算");
    }
    
    std::string resultUnit = hasUnit() ? unit_ : other.unit_;
    return CSSValue(value_ - other.value_, resultUnit);
}

inline CSSValue CSSValue::operator*(const CSSValue& other) const {
    // 乘法：至少一个无单位，或都无单位
    if (hasUnit() && other.hasUnit()) {
        throw std::runtime_error("乘法运算要求至少一个操作数无单位");
    }
    
    // 保留有单位的那个的单位
    std::string resultUnit = hasUnit() ? unit_ : other.unit_;
    return CSSValue(value_ * other.value_, resultUnit);
}

inline CSSValue CSSValue::operator/(const CSSValue& other) const {
    if (other.value_ == 0.0) {
        throw std::runtime_error("除数不能为零");
    }
    
    // 除法：右侧必须无单位
    if (other.hasUnit()) {
        throw std::runtime_error("除法运算的除数不能有单位");
    }
    
    return CSSValue(value_ / other.value_, unit_);
}

inline CSSValue CSSValue::operator%(const CSSValue& other) const {
    if (other.value_ == 0.0) {
        throw std::runtime_error("取模运算的除数不能为零");
    }
    
    if (!canMergeWith(other, '%')) {
        throw std::runtime_error("单位不匹配: 无法对 " + unit_ + " 和 " + 
                                 other.unit_ + " 进行取模运算");
    }
    
    std::string resultUnit = hasUnit() ? unit_ : other.unit_;
    return CSSValue(std::fmod(value_, other.value_), resultUnit);
}

inline CSSValue CSSValue::power(const CSSValue& other) const {
    // 幂运算：右侧必须无单位
    if (other.hasUnit()) {
        throw std::runtime_error("幂运算的指数不能有单位");
    }
    
    return CSSValue(std::pow(value_, other.value_), unit_);
}

inline CSSValue CSSValue::operator-() const {
    return CSSValue(-value_, unit_);
}

inline CSSValue CSSValue::operator+() const {
    return *this;
}

inline std::string CSSValue::toString() const {
    return formatValue(value_) + unit_;
}

inline bool CSSValue::canMergeWith(const CSSValue& other, char op) const {
    // 加减模：必须单位相同，或至少一个无单位
    if (op == '+' || op == '-' || op == '%') {
        // 都无单位 - OK
        if (!hasUnit() && !other.hasUnit()) {
            return true;
        }
        
        // 一个有单位，一个无单位 - OK（左结合）
        if (!hasUnit() || !other.hasUnit()) {
            return true;
        }
        
        // CHTL.md 第196-197行：百分号特殊处理
        // 1px + 5% 是合法的，百分号转为数字
        if ((unit_ == "%" || other.unit_ == "%") && 
            (unit_ != other.unit_)) {
            return true;  // 百分号可以与其他单位运算
        }
        
        // 都有单位 - 必须相同
        return unit_ == other.unit_;
    }
    
    return true;
}

inline std::string CSSValue::formatValue(double val) const {
    // 去除尾随零
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << val;
    std::string str = oss.str();
    
    // 如果有小数点，去除尾随零
    if (str.find('.') != std::string::npos) {
        str.erase(str.find_last_not_of('0') + 1);
        if (str.back() == '.') {
            str.pop_back();
        }
    }
    
    return str;
}

inline CSSValue CSSValue::fromString(const std::string& str) {
    // 解析字符串为 CSSValue
    // 例如: "100px" → CSSValue(100, "px")
    
    if (str.empty()) {
        return CSSValue();
    }
    
    size_t i = 0;
    
    // 跳过前导空格
    while (i < str.length() && std::isspace(str[i])) {
        i++;
    }
    
    // 解析符号
    bool negative = false;
    if (i < str.length() && (str[i] == '+' || str[i] == '-')) {
        negative = (str[i] == '-');
        i++;
    }
    
    // 解析数字部分
    std::string numStr;
    while (i < str.length() && (std::isdigit(str[i]) || str[i] == '.')) {
        numStr += str[i];
        i++;
    }
    
    if (numStr.empty()) {
        throw std::runtime_error("无效的CSS值: " + str);
    }
    
    double value = std::stod(numStr);
    if (negative) {
        value = -value;
    }
    
    // 解析单位部分
    std::string unit;
    while (i < str.length() && !std::isspace(str[i])) {
        unit += str[i];
        i++;
    }
    
    return CSSValue(value, unit);
}

} // namespace CHTL
