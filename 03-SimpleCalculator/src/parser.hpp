#include <memory>
#include <sstream>
#include "lexer.hpp"
#include "astNode.hpp"

/**
 * @brief 语法解析器
 */
class Parser
{
  public:
    Parser() : lexer_(std::make_unique<Lexer>())
    {
    }

  public:
    ASTNodePtr parse(const std::string& script)
    {
        lexer_->setInput(script);
        // 预读
        ahead_ = lexer_->nextToken();
        return prog();
    }

    /**
     * intDeclare
     *     : Int Identifier (Assignment additive)?
     *     ;
     *
     * additive
     *     : multiplicative
     *     | multiplicative Plus additive
     *     ;
     *
     * multiplicative
     *     : IntLiteral
     *     | IntLiteral Star multiplicative
     *     ;
     */
    ASTNodePtr prog()
    {
        if (ahead_.type == TokenType::Int)
        {
            return intDeclare();
        }
        // TokenType::IntLiteral
        else
        {
            return additive();
        }
    }

    /**
     * intDeclare : Int Identifier (Assignment additive)? ;
     */
    ASTNodePtr intDeclare()
    {
        // 匹配 'int'
        match(TokenType::Int);
        // 匹配标识符，并获取其名字
        auto id = match(TokenType::Identifier);
        // 生成节点
        auto result =
            std::make_shared<ASTNode>(ASTNodeType::IntDeclaration, id);
        // 判断是否有初始值
        if (ahead_.type == TokenType::Assignment)
        {
            // 匹配等号
            match(TokenType::Assignment);
            // 匹配加法表达式
            auto additiveNode = additive();
            // 将加法表达式作为子节点添加到结果节点中
            result->addChild(additiveNode);
        }
        return result;
    }

    /**
     * additive : multiplicative | multiplicative Plus additive ;
     */
    ASTNodePtr additive()
    {
        // 先生成左子节点
        auto child1 = multiplicative();
        // 判断下一个 token 是不是 "+"
        if (ahead_.type != TokenType::Plus)
        {
            // 不是的话直接将子节点返回
            return child1;
        }
        // 当前节点
        auto result =
            std::make_shared<ASTNode>(ASTNodeType::Additive, ahead_.value);
        // 左子节点
        result->addChild(child1);

        match(TokenType::Plus);
        // 右子节点
        auto child2 = additive();
        result->addChild(child2);

        return result;
    }

    /**
     * multiplicative : IntLiteral | IntLiteral Star multiplicative ;
     */
    ASTNodePtr multiplicative()
    {
        // 获取整数值字面量
        auto intValue = match(TokenType::IntLiteral);
        // 左子节点
        auto child1 =
            std::make_shared<ASTNode>(ASTNodeType::IntLiteral, intValue);
        // 不是 "*"，直接将左子节点返回
        if (ahead_.type != TokenType::Star)
        {
            return child1;
        }

        auto result = std::make_shared<ASTNode>(ASTNodeType::Multiplicative,
                                                ahead_.value);
        result->addChild(child1);

        match(TokenType::Star);

        auto child2 = multiplicative();
        result->addChild(child2);

        return result;
    }

    /**
     * 匹配下一个token，并返回其值。如果当前token类型不匹配，则抛出异常。
     */
    std::string match(TokenType type)
    {
        if (ahead_.type != type)
        {
            std::stringstream ss;
            ss << "parse error at pos("
               << lexer_->getPos() - ahead_.value.size() + 1 << "): expecting `"
               << type << "` but got `" << ahead_.type << "`.";
            throw std::runtime_error(ss.str());
        }
        const auto text = ahead_.value;
        ahead_ = lexer_->nextToken();
        return text;
    }

  private:
    std::unique_ptr<Lexer> lexer_;
    Token ahead_{TokenType::Unknown};  ///< 下一个token，预读
};
