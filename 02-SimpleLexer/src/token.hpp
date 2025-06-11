#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

/**
 * @brief Token 的类型
 *
 * 这里只定义了少量的 token 类型，和专栏第二节对应。
 */
enum class TokenType
{
    Identifier,  ///< [a-zA-Z_]([a-zA-Z_]|[0-9])*
    Int,         ///< 'int'
    IntLiteral,  ///< [0-9]+
    Assignment,  ///< '='
    GT,          ///< '>'
    GE,          ///< '>='
    Plus,        ///< '+'
    Minus,       ///< '-'
    Star,        ///< '*'
    Slash,       ///< '/'
    Unknown      ///< 未知
};

/**
 * @brief Token 类型的输出流重载
 */
#define TOKEN_TYPE_PRINT(type) \
    case type:                 \
        os << #type;           \
        return os;

inline std::ostream &operator<<(std::ostream &os, const TokenType &type)
{
    switch (type)
    {
        TOKEN_TYPE_PRINT(TokenType::Identifier)
        TOKEN_TYPE_PRINT(TokenType::Int)
        TOKEN_TYPE_PRINT(TokenType::IntLiteral)
        TOKEN_TYPE_PRINT(TokenType::Assignment)
        TOKEN_TYPE_PRINT(TokenType::GT)
        TOKEN_TYPE_PRINT(TokenType::GE)
        TOKEN_TYPE_PRINT(TokenType::Plus)
        TOKEN_TYPE_PRINT(TokenType::Minus)
        TOKEN_TYPE_PRINT(TokenType::Star)
        TOKEN_TYPE_PRINT(TokenType::Slash)
        TOKEN_TYPE_PRINT(TokenType::Unknown)
    }
    throw std::invalid_argument("Invalid TokenType");
}

#undef TOKEN_TYPE_PRINT

class Token
{
  public:
    /**
     * @brief Token 的构造函数
     *
     * @param type token 的类型
     * @param value token 的值
     */
    Token(TokenType type = TokenType::Unknown, const std::string &value = "")
        : type(type), value(value)
    {
    }

    /**
     * @brief Token转bool，有时会在循环中用到
     */
    explicit operator bool() const
    {
        return type != TokenType::Unknown;
    }

    TokenType type{TokenType::Unknown};
    std::string value;
};
