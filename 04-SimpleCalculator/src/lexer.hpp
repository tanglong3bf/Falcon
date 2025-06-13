#pragma once

#include <string>
#include <unordered_map>
#include "./token.hpp"

// 其中的_h后缀表示token解析了一半，如果我们支持了位运算，或许可以考虑改成BitOr
enum class DfaState
{
    Initial,     ///< 初始状态
    Id,          ///< 标识符
    Id_int1,     ///< 'i'
    Id_int2,     ///< 'in'
    Id_int3,     ///< 'int'
    Semicolon,   ///< ';'
    Or_h,        ///< '|'
    Or,          ///< '||'
    And_h,       ///< '&'
    And,         ///< '&&'
    Equal,       ///< '=='
    NotEqual_h,  ///< '!'
    NotEqual,    ///< '!='
    GT,          ///< '>'
    GE,          ///< '>='
    LT,          ///< '<'
    LE,          ///< '<='
    Plus,        ///< '+'
    Minus,       ///< '-'
    Star,        ///< '*'
    Slash,       ///< '/'
    Assignment,  ///< '='
    IntLiteral,  ///< 整数字面量
    LParen,      ///< '('
    RParen,      ///< ')'
};

/**
 * @brief 词法分析器
 * @details 实现一个简单的词法分析器，将输入字符串解析为Token序列。
 */
class Lexer
{
  public:
    Lexer(const std::string &input = "")
        : input_(input), state_(DfaState::Initial), pos_(0)
    {
    }

    void setInput(const std::string &input)
    {
        input_ = input;
        pos_ = 0;
        state_ = DfaState::Initial;
    }

    size_t getPos() const
    {
        return pos_;
    }

    void setPos(size_t pos)
    {
        pos_ = pos;
    }

    Token nextToken()
    {
        Token token{TokenType::Unknown};

        for (state_ = DfaState::Initial; pos_ < input_.size(); ++pos_)
        {
            const char c = input_[pos_];
            switch (state_)
            {
                case DfaState::Initial:
                    token = initToken(c);
                    break;
                case DfaState::Semicolon:
                case DfaState::GE:
                case DfaState::LE:
                case DfaState::Or:
                case DfaState::And:
                case DfaState::Equal:
                case DfaState::NotEqual:
                case DfaState::Plus:
                case DfaState::Minus:
                case DfaState::Star:
                case DfaState::Slash:
                case DfaState::LParen:
                case DfaState::RParen:
                    state_ = DfaState::Initial;
                    return token;
                case DfaState::Or_h:
                    if (c == '|')
                    {
                        token.type = TokenType::Or;
                        token.value += c;
                        state_ = DfaState::Or;
                    }
                    else
                    {
                        // 暂不支持位运算，一个|被认为是非法token
                        throw std::invalid_argument(
                            "Input `" + input_ +
                            "` has an invalid character: `" +
                            std::string(1, c) +
                            "` at pos: " + std::to_string(pos_ + 1));
                    }
                    break;
                case DfaState::And_h:
                    if (c == '&')
                    {
                        token.type = TokenType::And;
                        token.value += c;
                        state_ = DfaState::And;
                    }
                    else
                    {
                        // 暂不支持位运算，一个&被认为是非法token
                        throw std::invalid_argument(
                            "Input `" + input_ +
                            "` has an invalid character: `" +
                            std::string(1, c) +
                            "` at pos: " + std::to_string(pos_ + 1));
                    }
                    break;
                case DfaState::NotEqual_h:
                    if (c == '=')
                    {
                        token.type = TokenType::NotEqual;
                        token.value += c;
                        state_ = DfaState::NotEqual;
                    }
                    else
                    {
                        // 暂不支持感叹号
                        throw std::invalid_argument(
                            "Input `" + input_ +
                            "` has an invalid character: `" +
                            std::string(1, '!') +
                            "` at pos: " + std::to_string(pos_));
                    }
                    break;
                case DfaState::Assignment:
                    if (c == '=')
                    {
                        token.type = TokenType::Equal;
                        token.value += c;
                    }
                    else
                    {
                        return token;
                    }
                    break;
                case DfaState::Id:
                    if (std::isalnum(c) || c == '_')
                    {
                        token.value += c;
                    }
                    else
                    {
                        return token;
                    }
                    break;
                case DfaState::GT:
                    if (c == '=')
                    {
                        token.type = TokenType::GE;
                        token.value += c;
                        state_ = DfaState::GE;
                    }
                    else
                    {
                        return token;
                    }
                    break;
                case DfaState::LT:
                    if (c == '=')
                    {
                        token.type = TokenType::LE;
                        token.value += c;
                        state_ = DfaState::LE;
                    }
                    else
                    {
                        return token;
                    }
                    break;
                case DfaState::IntLiteral:
                    // 没有考虑特殊情况，比如 000123 会完整保留
                    if (isdigit(c))
                    {
                        token.value += c;
                    }
                    else
                    {
                        return token;
                    }
                    break;
                case DfaState::Id_int1:
                    if (c == 'n')
                    {
                        state_ = DfaState::Id_int2;
                        token.value += c;
                    }
                    else if (std::isalnum(c) || c == '_')
                    {
                        state_ = DfaState::Id;
                        token.value += c;
                    }
                    else
                    {
                        return token;
                    }
                    break;
                case DfaState::Id_int2:
                    if (c == 't')
                    {
                        state_ = DfaState::Id_int3;
                        token.value += c;
                    }
                    else if (std::isalnum(c) || c == '_')
                    {
                        state_ = DfaState::Id;
                        token.value += c;
                    }
                    else
                    {
                        return token;
                    }
                    break;
                case DfaState::Id_int3:
                    if (isalnum(c) || c == '_')
                    {
                        state_ = DfaState::Id;
                        token.value += c;
                    }
                    else
                    {
                        token.type = TokenType::Int;
                        return token;
                    }
                    break;
            }
        }
        return token;
    }

    bool done()
    {
        return pos_ >= input_.size();
    }

  private:
    /**
     * @brief 处理一个字符，更新状态
     */
    Token initToken(char c)
    {
        // token的第一个字符直接转移到对应状态
        static const std::unordered_map<char, DfaState> stateMap = {
            {'+', DfaState::Plus},
            {'-', DfaState::Minus},
            {'*', DfaState::Star},
            {'/', DfaState::Slash},
            {'=', DfaState::Assignment},
            {'>', DfaState::GT},
            {'<', DfaState::LT},
            {'|', DfaState::Or_h},
            {'&', DfaState::And_h},
            {'!', DfaState::NotEqual_h},
            {';', DfaState::Semicolon},
            {'(', DfaState::LParen},
            {')', DfaState::RParen},
        };
        // 第一个字符直接设置token的类型
        static const std::unordered_map<char, TokenType> tokenTypeMap = {
            {'+', TokenType::Plus},
            {'-', TokenType::Minus},
            {'*', TokenType::Star},
            {'/', TokenType::Slash},
            {'=', TokenType::Assignment},
            {'>', TokenType::GT},
            {'<', TokenType::LT},
            {'|', TokenType::Or},
            {'&', TokenType::And},
            {'!', TokenType::NotEqual},
            {';', TokenType::Semicolon},
            {'(', TokenType::LParen},
            {')', TokenType::RParen},
        };
        // 空格、制表符、换行符，直接忽略
        if (std::isspace(c))
        {
            state_ = DfaState::Initial;
            return Token{TokenType::Unknown};
        }
        // 简单的情况直接处理
        switch (c)
        {
            case '+':
            case '-':
            case '*':
            case '/':
            case '=':
            case '>':
            case '<':
            case '|':
            case '&':
            case '!':
            case ';':
            case '(':
            case ')':
                state_ = stateMap.at(c);
                return Token{tokenTypeMap.at(c), std::string(1, c)};
        }
        // 数字字面量
        if (isdigit(c))
        {
            state_ = DfaState::IntLiteral;
            return Token{TokenType::IntLiteral, std::string(1, c)};
        }
        // 标识符
        else if (std::isalpha(c))
        {
            if (c == 'i')
            {
                state_ = DfaState::Id_int1;
            }
            else
            {
                state_ = DfaState::Id;
            }
            return Token{TokenType::Identifier, std::string(1, c)};
        }
        // 不认识的符号，报错
        throw std::invalid_argument(
            "Input `" + input_ + "` has an invalid character: `" +
            std::string(1, c) + "` at pos: " + std::to_string(pos_ + 1));
    }

    std::string input_;
    DfaState state_ = DfaState::Initial;
    size_t pos_;
};
