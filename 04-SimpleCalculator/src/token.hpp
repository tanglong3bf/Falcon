#pragma once

#include <cstring>
#include <iostream>
#include <string>

/**
 * @brief Token 的类型
 *
 * 补充了一些运算符
 */
enum class TokenType
{
    Identifier,  ///< [a-zA-Z_]([a-zA-Z_]|[0-9])*
    Int,         ///< 'int'
    IntLiteral,  ///< [0-9]+
    Semicolon,   ///< ';'
    Assignment,  ///< '='
    Or,          ///< '||'
    And,         ///< '&&'
    Equal,       ///< '=='
    NotEqual,    ///< '!='
    GT,          ///< '>'
    GE,          ///< '>='
    LT,          ///< '<'
    LE,          ///< '<='
    Plus,        ///< '+'
    Minus,       ///< '-'
    Star,        ///< '*'
    Slash,       ///< '/'
    LParen,      ///< '('
    RParen,      ///< ')'
    Unknown      ///< 未知
};

/**
 * @brief Token 类型的输出流重载
 *
 * 这种修改后的不会报警告了
 */
#define TOKEN_TYPE_PRINT(type)                      \
    case type:                                      \
    {                                               \
        static const char *fullStr = #type;         \
        os << fullStr + std::strlen("TokenType::"); \
        break;                                      \
    }

inline std::ostream &operator<<(std::ostream &os, const TokenType &type)
{
    switch (type)
    {
        TOKEN_TYPE_PRINT(TokenType::Identifier)
        TOKEN_TYPE_PRINT(TokenType::Int)
        TOKEN_TYPE_PRINT(TokenType::IntLiteral)
        TOKEN_TYPE_PRINT(TokenType::Semicolon)
        TOKEN_TYPE_PRINT(TokenType::Assignment)
        TOKEN_TYPE_PRINT(TokenType::Or)
        TOKEN_TYPE_PRINT(TokenType::And)
        TOKEN_TYPE_PRINT(TokenType::Equal)
        TOKEN_TYPE_PRINT(TokenType::NotEqual)
        TOKEN_TYPE_PRINT(TokenType::GT)
        TOKEN_TYPE_PRINT(TokenType::GE)
        TOKEN_TYPE_PRINT(TokenType::LT)
        TOKEN_TYPE_PRINT(TokenType::LE)
        TOKEN_TYPE_PRINT(TokenType::Plus)
        TOKEN_TYPE_PRINT(TokenType::Minus)
        TOKEN_TYPE_PRINT(TokenType::Star)
        TOKEN_TYPE_PRINT(TokenType::Slash)
        TOKEN_TYPE_PRINT(TokenType::LParen)
        TOKEN_TYPE_PRINT(TokenType::RParen)
        TOKEN_TYPE_PRINT(TokenType::Unknown)
    }
    return os;
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
