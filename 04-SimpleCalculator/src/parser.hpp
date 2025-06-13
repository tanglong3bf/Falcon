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
     * prog ::= (intDeclare | assign) ";"
     * intDeclare ::= "int" Id ( "=" assign )?
     * assign ::= orExp ( "=" assign )?
     * orExp ::= andExp ( "||" andExp )*
     * andExp ::= equalExp ( "&&" equalExp )*
     * equalExp ::= relExp ( ("==" | "!=") relExp )*
     * relExp ::= addExp ( (">" | "<" | ">=" | "<=") addExp )*
     * addExp ::= mulExp ( ("+" | "-") mulExp )*
     * mulExp ::= priExp ( ("*" | "/") priExp )*
     * priExp ::= Id | Literal | "(" assign ")"
     */
    ASTNodePtr prog()
    {
        ASTNodePtr result = nullptr;
        if (ahead_.type == TokenType::Int)
        {
            result = intDeclare();
        }
        else
        {
            result = assign();
        }
        match(TokenType::Semicolon);
        return result;
    }

    /**
     * intDeclare ::= "int" Id ( "=" assign )?
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
            // 匹配表达式
            auto assignNode = assign();
            // 将加法表达式作为子节点添加到结果节点中
            result->addChild(assignNode);
        }
        return result;
    }

    /**
     * assign ::= orExp ( "=" assign )?
     */
    ASTNodePtr assign()
    {
        auto orNode = orExp();
        if (ahead_.type == TokenType::Assignment)
        {
            match(TokenType::Assignment);
            auto assignNode = assign();
            auto assignmentNode =
                std::make_shared<ASTNode>(ASTNodeType::Assignment, "=");
            assignmentNode->addChild(orNode);
            assignmentNode->addChild(assignNode);
            return assignmentNode;
        }
        return orNode;
    }

    /**
     * orExp ::= andExp ( "||" andExp )*
     */
    ASTNodePtr orExp()
    {
        auto andNode = andExp();
        while (ahead_.type == TokenType::Or)
        {
            auto orNode =
                std::make_shared<ASTNode>(ASTNodeType::Logical, ahead_.value);
            orNode->addChild(andNode);
            match(TokenType::Or);
            andNode = andExp();
            orNode->addChild(andNode);
            andNode = orNode;
        }
        return andNode;
    }

    /**
     * andExp ::= equalExp ( "&&" equalExp )*
     */
    ASTNodePtr andExp()
    {
        auto equalNode = equalExp();
        while (ahead_.type == TokenType::And)
        {
            auto andNode =
                std::make_shared<ASTNode>(ASTNodeType::Logical, ahead_.value);
            andNode->addChild(equalNode);
            match(TokenType::And);
            equalNode = equalExp();
            andNode->addChild(equalNode);
            equalNode = andNode;
        }
        return equalNode;
    }

    /**
     * equalExp ::= relExp ( ("==" | "!=") relExp )*
     */
    ASTNodePtr equalExp()
    {
        auto relNode = relExp();
        while (ahead_.type == TokenType::Equal ||
               ahead_.type == TokenType::NotEqual)
        {
            auto equalNode = std::make_shared<ASTNode>(ASTNodeType::Relational,
                                                       ahead_.value);
            equalNode->addChild(relNode);
            match(ahead_.type);
            relNode = relExp();
            equalNode->addChild(relNode);
            relNode = equalNode;
        }
        return relNode;
    }

    /**
     * relExp ::= addExp ( (">" | "<" | ">=" | "<=") addExp )*
     */
    ASTNodePtr relExp()
    {
        auto addNode = addExp();
        while (ahead_.type == TokenType::GT || ahead_.type == TokenType::LT ||
               ahead_.type == TokenType::GE || ahead_.type == TokenType::LE)
        {
            auto relNode = std::make_shared<ASTNode>(ASTNodeType::Relational,
                                                     ahead_.value);
            relNode->addChild(addNode);
            match(ahead_.type);
            addNode = addExp();
            relNode->addChild(addNode);
            addNode = relNode;
        }
        return addNode;
    }

    /**
     * addExp ::= mulExp ( ("+" | "-") mulExp )*
     */
    ASTNodePtr addExp()
    {
        auto mulNode = mulExp();
        while (ahead_.type == TokenType::Plus ||
               ahead_.type == TokenType::Minus)
        {
            auto addNode =
                std::make_shared<ASTNode>(ASTNodeType::Additive, ahead_.value);
            addNode->addChild(mulNode);
            match(ahead_.type);
            mulNode = mulExp();
            addNode->addChild(mulNode);
            mulNode = addNode;
        }
        return mulNode;
    }

    /**
     * mulExp ::= priExp ( ("*" | "/") priExp )*
     */
    ASTNodePtr mulExp()
    {
        auto priNode = priExp();
        while (ahead_.type == TokenType::Star ||
               ahead_.type == TokenType::Slash)
        {
            auto mulNode =
                std::make_shared<ASTNode>(ASTNodeType::Multiplicative,
                                          ahead_.value);
            mulNode->addChild(priNode);
            match(ahead_.type);
            priNode = priExp();
            mulNode->addChild(priNode);
            priNode = mulNode;
        }
        return priNode;
    }

    /**
     * priExp ::= Id | Literal | "(" assign ")"
     */
    ASTNodePtr priExp()
    {
        if (ahead_.type == TokenType::Identifier)
        {
            auto id = match(TokenType::Identifier);
            return std::make_shared<ASTNode>(ASTNodeType::Identifier, id);
        }
        else if (ahead_.type == TokenType::IntLiteral)
        {
            auto literal = match(TokenType::IntLiteral);
            return std::make_shared<ASTNode>(ASTNodeType::IntLiteral, literal);
        }
        else if (ahead_.type == TokenType::LParen)
        {
            match(TokenType::LParen);
            auto assignNode = assign();
            match(TokenType::RParen);
            return assignNode;
        }
        else
        {
            std::stringstream ss;
            ss << "parse error at pos(" << lexer_->getPos()
               << "): unexpected token `" << ahead_.value << "`.";
            throw std::runtime_error(ss.str());
        }
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
