#pragma once

#include <cstdlib>
#include "./generated/FalconScriptBaseVisitor.h"
#include <sstream>

/**
 * 普通二元运算符
 */
#define BINARY_OPERATOR(op_name, op)                                          \
    case FalconScriptParser::op_name:                                         \
        if (left.is<int>() && right.is<int>())                                \
        {                                                                     \
            result = static_cast<int32_t>(left.as<int>() op right.as<int>()); \
        }                                                                     \
        else if (left.is<int>() && right.is<int *>())                         \
        {                                                                     \
            result =                                                          \
                static_cast<int32_t>(left.as<int>() op * right.as<int *>());  \
        }                                                                     \
        else if (left.is<int *>() && right.is<int>())                         \
        {                                                                     \
            result =                                                          \
                static_cast<int32_t>(*left.as<int *>() op right.as<int>());   \
        }                                                                     \
        else if (left.is<int *>() && right.is<int *>())                       \
        {                                                                     \
            result = static_cast<int32_t>(*left.as<int *>() op *              \
                                          right.as<int *>());                 \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            throw std::runtime_error("类型不匹配");                           \
        }                                                                     \
        break

/**
 * 赋值运算符
 *
 * 如果是repl模式且不在循环中，则输出变量的新值
 */
#define ASSIGN_OPERATOR(op_name, op)                                          \
    case FalconScriptParser::op_name:                                         \
        if (left.is<int32_t>())                                               \
        {                                                                     \
            throw std::runtime_error("赋值号左侧不能为字面量");               \
        }                                                                     \
        if (right.is<int32_t>())                                              \
        {                                                                     \
            result = static_cast<int32_t>(*left.as<int32_t *>()               \
                                               op right.as<int32_t>());       \
        }                                                                     \
        else if (right.is<int32_t *>())                                       \
        {                                                                     \
            result = static_cast<int32_t>(*left.as<int32_t *>() op *          \
                                          right.as<int32_t *>());             \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            throw std::runtime_error("类型不匹配");                           \
        }                                                                     \
        if (isRepl_ && loopDepth_ == 0)                                       \
        {                                                                     \
            std ::cout << leftName << ": " << result.as<int>() << std ::endl; \
        }                                                                     \
        break

/**
 * 前缀运算符
 */
#define PREFIX_UNARY_OPERATOR(op_name, op)                             \
    case FalconScriptParser::op_name:                                  \
        if (child.is<int32_t>())                                       \
        {                                                              \
            result = static_cast<int32_t>(op child.as<int32_t>());     \
        }                                                              \
        else if (child.is<int32_t *>())                                \
        {                                                              \
            result = static_cast<int32_t>(op * child.as<int32_t *>()); \
        }                                                              \
        else                                                           \
        {                                                              \
            throw std::runtime_error("类型不匹配");                    \
        }                                                              \
        break

/**
 * 支持的变量类型
 */
enum class FalconType
{
    Integer,  ///< int32_t*
    Array,  ///< std::vector<antlrcpp::Any>* // 可能直接存储数字，可能有子数组
};

/**
 * 一些用于控制流程的语句
 */
enum class StatementFlowControl
{
    Continue,
    Break
};

class MyVisitor : public FalconScriptBaseVisitor
{
  public:
    MyVisitor(bool isRepl) : isRepl_(isRepl), loopDepth_(0)
    {
    }

  public:
    /**
     * 程序的入口，遍历所有语句，如果有错误，则输出错误信息，停止运行
     */
    virtual antlrcpp::Any visitProg(
        FalconScriptParser::ProgContext *ctx) override
    {
        // try
        // {
        visitChildren(ctx);
        // }
        // catch (std::exception &e)
        // {
        //     std::cout << "Error: " << e.what() << std::endl;
        // }
        return nullptr;
    }

    /**
     * 被花括号包裹的语句块，如果中间有语句返回了Break或Continue，则将结果返回给外层，由外层处理
     */
    virtual antlrcpp::Any visitBlock(
        FalconScriptParser::BlockContext *ctx) override
    {
        for (auto statement : ctx->blockStatement())
        {
            auto result = visitBlockStatement(statement);
            if (result.is<StatementFlowControl>() &&
                (result.as<StatementFlowControl>() ==
                     StatementFlowControl::Break ||
                 result.as<StatementFlowControl>() ==
                     StatementFlowControl::Continue))
            {
                return result;
            }
        }
        return nullptr;
    }

    virtual antlrcpp::Any visitBlockStatement(
        FalconScriptParser::BlockStatementContext *ctx) override
    {
        if (ctx->statement())
        {
            return visitStatement(ctx->statement());
        }
        else if (ctx->variableDeclarators())
        {
            visitVariableDeclarators(ctx->variableDeclarators());
        }
        return nullptr;
    }

    /**
     * 基本语句
     */
    virtual antlrcpp::Any visitStatement(
        FalconScriptParser::StatementContext *ctx) override
    {
        // 花括号包裹的语句块
        if (ctx->blockLabel)
        {
            return visitBlock(ctx->blockLabel);
        }
        else if (ctx->IF())
        {
            int32_t condition;
            auto value = visitParExpression(ctx->parExpression());
            if (value.is<int32_t>())
            {
                condition = value.as<int32_t>();
            }
            else if (value.is<int32_t *>())
            {
                condition = *value.as<int32_t *>();
            }
            if (condition != 0)
            {
                return visitStatement(ctx->statement(0));
            }
            else if (ctx->ELSE())
            {
                return visitStatement(ctx->statement(1));
            }
        }
        else if (ctx->FOR())
        {
            ++loopDepth_;
            auto forControl = ctx->forControl();
            if (forControl->forInit())
            {
                visitForInit(forControl->forInit());
            }
            while (true)
            {
                if (forControl->expression())
                {
                    auto condition /*:int32_t*/
                        = visitExpression(forControl->expression()).as<int>();
                    if (condition == 0)
                        break;
                }
                auto result = visitStatement(ctx->statement(0));
                if (result.is<StatementFlowControl>() &&
                    result.as<StatementFlowControl>() ==
                        StatementFlowControl::Break)
                {
                    break;
                }
                if (result.is<StatementFlowControl>() &&
                    result.as<StatementFlowControl>() ==
                        StatementFlowControl::Continue)
                {
                    continue;
                }
                if (forControl->forUpdate)
                {
                    visitExpressionList(forControl->forUpdate);
                }
            }
            --loopDepth_;
        }
        else if (ctx->WHILE() && ctx->DO() == nullptr)
        {
            ++loopDepth_;
            while (true)
            {
                auto condition /*:int32_t*/ =
                    visitParExpression(ctx->parExpression()).as<int>();
                if (condition == 0)
                {
                    break;
                }
                auto result = visitStatement(ctx->statement(0));
                if (result.is<StatementFlowControl>() &&
                    result.as<StatementFlowControl>() ==
                        StatementFlowControl::Break)
                {
                    break;
                }
                if (result.is<StatementFlowControl>() &&
                    result.as<StatementFlowControl>() ==
                        StatementFlowControl::Continue)
                {
                    continue;
                }
            }
            --loopDepth_;
        }
        else if (ctx->DO())
        {
            ++loopDepth_;
            while (true)
            {
                auto result = visitStatement(ctx->statement(0));
                if (result.is<StatementFlowControl>() &&
                    result.as<StatementFlowControl>() ==
                        StatementFlowControl::Break)
                {
                    break;
                }
                if (result.is<StatementFlowControl>() &&
                    result.as<StatementFlowControl>() ==
                        StatementFlowControl::Continue)
                {
                    continue;
                }
                auto condition /*:int32_t*/ =
                    visitParExpression(ctx->parExpression()).as<int>();
                if (condition == 0)
                {
                    break;
                }
            }
            --loopDepth_;
        }
        // 递归检查父节点是否是循环语句，如果是，则返回Break或Continue
        else if (ctx->BREAK())
        {
            auto parent = ctx->parent;
            while (parent)
            {
                auto statement =
                    dynamic_cast<FalconScriptParser::StatementContext *>(
                        parent);
                if (statement && (statement->FOR() || statement->WHILE()))
                {
                    return StatementFlowControl::Break;
                }
                parent = parent->parent;
            }
            std::cout << "\033[33mWarning: \033[0mbreak不在循环中，已忽略"
                      << std::endl;
        }
        else if (ctx->CONTINUE())
        {
            auto parent = ctx->parent;
            while (parent)
            {
                auto statement =
                    dynamic_cast<FalconScriptParser::StatementContext *>(
                        parent);
                if (statement && (statement->FOR() || statement->WHILE()))
                {
                    return StatementFlowControl::Continue;
                }
                parent = parent->parent;
            }
            std::cout << "\033[33mWarning: \033[0mcontinue不在循环中，已忽略"
                      << std::endl;
        }
        else if (ctx->statementExpression)
        {
            auto result = visitExpression(ctx->statementExpression);
            // 类似于 a; 的语句，输出变量的值
            if (ctx->statementExpression->primary())
            {
                assert(result.is<int32_t *>());
                std::cout << ctx->statementExpression->getText() << ": "
                          << *result.as<int *>() << std::endl;
            }
            else if (ctx->statementExpression->bop != nullptr)
            {
                auto type = ctx->statementExpression->bop->getType();
                switch (type)
                {
                    case FalconScriptParser::ASSIGN:
                    case FalconScriptParser::PLUS_ASSIGN:
                    case FalconScriptParser::MINUS_ASSIGN:
                    case FalconScriptParser::MULTIPLY_ASSIGN:
                    case FalconScriptParser::DIVIDE_ASSIGN:
                    case FalconScriptParser::MODULUS_ASSIGN:
                    case FalconScriptParser::L_SHIFT_ASSIGN:
                    case FalconScriptParser::R_SHIFT_ASSIGN:
                    case FalconScriptParser::BIT_AND_ASSIGN:
                    case FalconScriptParser::BIT_OR_ASSIGN:
                    case FalconScriptParser::BIT_XOR_ASSIGN:
                        break;
                    default:
                        if (isRepl_ && loopDepth_ == 0)
                        {
                            // 非赋值的二元运算符的计算结果输出
                            std::cout << ctx->statementExpression->getText()
                                      << ": " << result.as<int>() << std::endl;
                        }
                }
            }
            return result;
        }
        return nullptr;
    }

    virtual antlrcpp::Any /* int32_t, int32_t* */ visitExpression(
        FalconScriptParser::ExpressionContext *ctx) override
    {
        // 缓存所有的表达式结果
        antlrcpp::Any result;
        if (ctx->primary())
        {
            result /* int32_t, int32_t* */ = visitPrimary(ctx->primary());
        }
        // 双目运算符
        else if (ctx->bop != nullptr && ctx->expression().size() == 2)
        {
            // 获取左右表达式的结果
            antlrcpp::Any left(visitExpression(ctx->expression(0)));
            antlrcpp::Any right(visitExpression(ctx->expression(1)));
            assert(left.is<int32_t>() ||
                   left.is<int32_t *>() && left.as<int32_t *>() != nullptr);
            assert(right.is<int32_t>() ||
                   right.is<int32_t *>() && right.as<int32_t *>() != nullptr);

            std::string leftName;
            // 根据运算符类型进行计算
            switch (ctx->bop->getType())
            {
                BINARY_OPERATOR(PLUS, +);
                BINARY_OPERATOR(MINUS, -);
                BINARY_OPERATOR(MULTIPLY, *);
                BINARY_OPERATOR(DIVIDE, /);
                BINARY_OPERATOR(MODULUS, %);
                BINARY_OPERATOR(L_SHIFT, <<);
                BINARY_OPERATOR(R_SHIFT, >>);
                BINARY_OPERATOR(EQUAL, ==);
                BINARY_OPERATOR(NOT_EQUAL, !=);
                BINARY_OPERATOR(GREATER, >);
                BINARY_OPERATOR(LESS, <);
                BINARY_OPERATOR(GREATER_EQUAL, >=);
                BINARY_OPERATOR(LESS_EQUAL, <=);
                BINARY_OPERATOR(BIT_AND, &);
                BINARY_OPERATOR(BIT_OR, |);
                BINARY_OPERATOR(BIT_XOR, ^);
                BINARY_OPERATOR(AND, &&);
                BINARY_OPERATOR(OR, ||);
                default:
                    // 赋值号需要获取左边的变量名
                    leftName =
                        ctx->expression(0)->primary()->IDENTIFIER()->getText();
                    break;
            }
            // 赋值号
            switch (ctx->bop->getType())
            {
                // 这里会用到 leftName 修改内存中的变量值
                ASSIGN_OPERATOR(ASSIGN, =);
                ASSIGN_OPERATOR(PLUS_ASSIGN, +=);
                ASSIGN_OPERATOR(MINUS_ASSIGN, -=);
                ASSIGN_OPERATOR(MULTIPLY_ASSIGN, *=);
                ASSIGN_OPERATOR(DIVIDE_ASSIGN, /=);
                ASSIGN_OPERATOR(MODULUS_ASSIGN, %=);
                ASSIGN_OPERATOR(L_SHIFT_ASSIGN, <<=);
                ASSIGN_OPERATOR(R_SHIFT_ASSIGN, >>=);
                ASSIGN_OPERATOR(BIT_AND_ASSIGN, &=);
                ASSIGN_OPERATOR(BIT_OR_ASSIGN, |=);
                ASSIGN_OPERATOR(BIT_XOR_ASSIGN, ^=);
                default:
                    break;
            }
        }
        // 前置单目运算符
        else if (ctx->prefix != nullptr && ctx->expression().size() == 1)
        {
            antlrcpp::Any child(visitExpression(ctx->expression(0)));
            switch (ctx->prefix->getType())
            {
                PREFIX_UNARY_OPERATOR(PLUS, +);
                PREFIX_UNARY_OPERATOR(MINUS, -);
                PREFIX_UNARY_OPERATOR(NOT, !);
                PREFIX_UNARY_OPERATOR(NEGATE, ~);
            }
            switch (ctx->prefix->getType())
            {
                case FalconScriptParser::INCREMENT:
                    assert(child.is<int32_t *>());
                    result = ++*child.as<int32_t *>();
                    break;
                case FalconScriptParser::DECREMENT:
                    assert(child.is<int32_t *>());
                    result = --*child.as<int32_t *>();
                    break;
            }
        }
        // 后置单目运算符
        else if (ctx->postfix != nullptr)
        {
            antlrcpp::Any child(visitExpression(ctx->expression(0)));
            switch (ctx->postfix->getType())
            {
                case FalconScriptParser::INCREMENT:
                    assert(child.is<int32_t *>());
                    result = (*child.as<int32_t *>())++;
                    break;
                case FalconScriptParser::DECREMENT:
                    assert(child.is<int32_t *>());
                    result = (*child.as<int32_t *>())--;
                    break;
            }
        }
        // 三目运算符
        else if (ctx->bop != nullptr && ctx->expression().size() == 3 &&
                 ctx->bop->getType() == FalconScriptParser::TERNARY)
        {
            antlrcpp::Any cond(visitExpression(ctx->expression(0)));
            assert(cond.is<int32_t>());
            if (cond.as<int32_t>() != 0)
            {
                result = visitExpression(ctx->expression(1));
            }
            else
            {
                result = visitExpression(ctx->expression(2));
            }
        }

        return result;
    }

    virtual antlrcpp::Any /* @see visitExpression, int32_t, int32_t* */
    visitPrimary(FalconScriptParser::PrimaryContext *ctx) override
    {
        if (ctx->L_PAREN() && ctx->R_PAREN())
        {
            return visitExpression(ctx->expression());
        }
        else if (ctx->literal())
        {
            return visitLiteral(ctx->literal());
        }
        else  // IDENTIFIER
        {
            auto varName = ctx->IDENTIFIER()->getText();
            if (variables_.find(varName) != variables_.end())
            {
                return variables_[varName];
            }
            else
            {
                // 变量未定义，报错
                std::stringstream ss;
                ss << "变量" << varName << "未定义";
                throw std::runtime_error(ss.str());
            }
        }
    }

    virtual antlrcpp::Any /* int32_t */ visitLiteral(
        FalconScriptParser::LiteralContext *ctx) override
    {
        return visitIntegerLiteral(ctx->integerLiteral());
    }

    virtual antlrcpp::Any /* int32_t */ visitIntegerLiteral(
        FalconScriptParser::IntegerLiteralContext *ctx) override
    {
        if (ctx->DECIMAL_LITERAL())
        {
            return std::atoi(ctx->DECIMAL_LITERAL()->getText().c_str());
        }
        else if (ctx->HEX_LITERAL())
        {
            return static_cast<int>(std::strtoul(
                ctx->HEX_LITERAL()->getText().c_str(), nullptr, 16));
        }
        else if (ctx->OCTAL_LITERAL())
        {
            return static_cast<int>(std::strtoul(
                ctx->OCTAL_LITERAL()->getText().c_str(), nullptr, 8));
        }
        else  // ctx->BINARY_LITERAL()
        {
            return static_cast<int>(std::strtoul(
                ctx->BINARY_LITERAL()->getText().c_str(), nullptr, 2));
        }
    }

    virtual antlrcpp::Any /* FalconType */ visitTypeType(
        FalconScriptParser::TypeTypeContext *ctx) override
    {
        if (ctx->primitiveType())
        {
            return visitPrimitiveType(ctx->primitiveType());
        }
        throw std::runtime_error("未知类型");
    }

    virtual antlrcpp::Any /* FalconType */ visitPrimitiveType(
        FalconScriptParser::PrimitiveTypeContext *ctx) override
    {
        if (ctx->INT())
        {
            return FalconType::Integer;
        }
        return nullptr;
    }

    virtual antlrcpp::Any /* std::nullptr_t */ visitVariableDeclarators(
        FalconScriptParser::VariableDeclaratorsContext *ctx) override
    {
        for (auto declarator : ctx->variableDeclarator())
        {
            visitVariableDeclarator(declarator);
        }
        return nullptr;
    }

    virtual antlrcpp::Any /* std::nullptr_t */ visitVariableDeclarator(
        FalconScriptParser::VariableDeclaratorContext *ctx) override
    {
        auto varName /*:string*/ =
            visitVariableDeclaratorId(ctx->variableDeclaratorId());
        if (variables_.find(varName.as<std::string>()) != variables_.end())
        {
            std::stringstream ss;
            ss << "变量" << varName.as<std::string>() << "已定义";
            throw std::runtime_error(ss.str());
        }
        int value = 0;
        if (ctx->variableInitializer())
        {
            value = visitVariableInitializer(ctx->variableInitializer());
        }
        variables_[varName.as<std::string>()] = new int(value);
        // 新定义的变量输出一下
        if (isRepl_ && loopDepth_ == 0)
        {
            std::cout << varName.as<std::string>() << ": " << value
                      << std::endl;
        }

        return nullptr;
    }

    virtual antlrcpp::Any /* std::string (标识符) */ visitVariableDeclaratorId(
        FalconScriptParser::VariableDeclaratorIdContext *ctx) override
    {
        return ctx->IDENTIFIER()->getText();
    }

    virtual antlrcpp::Any /* @see visitExpression */ visitVariableInitializer(
        FalconScriptParser::VariableInitializerContext *ctx) override
    {
        return visitExpression(ctx->expression());
    }

    virtual antlrcpp::Any /* @see visitExpression */ visitParExpression(
        FalconScriptParser::ParExpressionContext *ctx) override
    {
        return visitExpression(ctx->expression());
    }

    virtual antlrcpp::Any /* std::nullptr_t */ visitForInit(
        FalconScriptParser::ForInitContext *ctx) override
    {
        if (ctx->variableDeclarators())
        {
            visitVariableDeclarators(ctx->variableDeclarators());
        }
        else if (ctx->expressionList())
        {
            visitExpressionList(ctx->expressionList());
        }
        return nullptr;
    }

    virtual antlrcpp::Any /* std::nullptr_t */ visitExpressionList(
        FalconScriptParser::ExpressionListContext *ctx) override
    {
        for (auto expression : ctx->expression())
        {
            visitExpression(expression);
        }
        return nullptr;
    }

  private:
    /**
     * 可能的类型：int32_t*, std::vector<antlrcpp::Any>
     * 数组内部也是存储上述两种类型
     */
    static std::unordered_map<std::string, antlrcpp::Any> variables_;
    /// isRepl_ 是否处于REPL模式
    const bool isRepl_;
    /// loopDepth_ 记录当前所在的循环层级
    int loopDepth_;
};

inline std::unordered_map<std::string, antlrcpp::Any> MyVisitor::variables_ =
    {};

#undef BINARY_OPERATOR
#undef PREFIX_UNARY_OPERATOR
