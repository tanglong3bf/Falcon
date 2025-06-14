#pragma once

#include "astNode.hpp"
#include "parser.hpp"
#include <unordered_map>

class Repl
{
  public:
    void run()
    {
        Parser parser;
        std::cout << "Welcome to Falcon!" << std::endl;
        std::cout << "> ";
        std::string buffer;
        std::string input;
        while (std::getline(std::cin, input))
        {
            // 检查是否退出
            // exit();如果被写到了多行里，不会生效
            if (input == "exit();")
            {
                std::cout << "bye!" << std::endl;
                break;
            }
            // 当前行追加到buffer里
            buffer += input + ' ';
            // 如果输入没有以;结尾，则继续等待输入
            auto i = input.find_last_not_of(' ');
            if (input[i] != ';')
            {
                std::cout << "> ";
                continue;
            }
            try
            {
                auto ast = parser.parse(buffer);
                for (const auto &child : ast->children_)
                {
                    // 打印抽象语法树
                    if (verbose_)
                    {
                        child->print();
                    }
                    auto result = evaluate(child);
                    // 如果根节点是赋值语句，evaluate函数内部已经缓存完结果了
                    // 不是的话，我们再存储一下
                    if (child->getType() != ASTNodeType::Assignment)
                    {
                        results_.emplace_back(result);
                    }
                    for (const auto &result : results_)
                    {
                        if (result.isNewVariable)
                        {
                            std::cout << "(*)";
                        }
                        if (result.variableName != "")
                        {
                            std::cout << result.variableName << ": ";
                        }
                        std::cout << result.value << std::endl;
                    }
                    results_.clear();
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "\033[31mError: \033[0m" << e.what() << std::endl;
            }
            // 清空buffer
            buffer = "";
            std::cout << "\n> ";
        }
    }

    void setVerbose(bool verbose)
    {
        verbose_ = verbose;
    }

  private:
    struct EvaluatorResult
    {
        int value;
        std::string variableName = "";
        bool isNewVariable = false;
    };

// 递归调用之后，用.value取值后进行计算
#define BOP_CASE(op)                                       \
    if (node->value_ == #op)                               \
    {                                                      \
        return {evaluate(node->children_[0])               \
                    .value op evaluate(node->children_[1]) \
                    .value};                               \
    }
#define ELSE_BOP_CASE(op) else BOP_CASE(op)

    /**
     * @brief 计算抽象语法树节点的值
     */
    EvaluatorResult evaluate(const ASTNodePtr &node)
    {
        switch (node->type_)
        {
            case ASTNodeType::Program:
                std::cerr << "不要走这" << std::endl;
                return {0};
            case ASTNodeType::IntDeclaration:
                if (variables_.find(node->value_) == variables_.end())
                {
                    if (node->children_.size() == 1)
                    {
                        auto result = evaluate(node->children_[0]);
                        variables_[node->value_] = result.value;
                        return {result.value, node->value_, true};
                    }
                    else
                    {
                        variables_[node->value_] = 0;
                        return {0, node->value_, true};
                    }
                }
                throw std::runtime_error("variable '" + node->value_ +
                                         "' has been defined.");
            case ASTNodeType::Assignment:
                if (variables_.find(node->value_) != variables_.end())
                {
                    auto r = evaluate(node->children_[0]);
                    variables_[node->value_] = r.value;
                    EvaluatorResult result = {r.value, node->value_};
                    // 缓存一下赋值语句的执行结果
                    results_.emplace_back(result);
                    return result;
                }
                throw std::runtime_error("variable '" + node->value_ +
                                         "' is not defined.");
            case ASTNodeType::Logical:
                BOP_CASE(&&)
                ELSE_BOP_CASE(||)
                break;
            case ASTNodeType::Relational:
                BOP_CASE(==)
                ELSE_BOP_CASE(!=)
                ELSE_BOP_CASE(<)
                ELSE_BOP_CASE(>)
                ELSE_BOP_CASE(<=)
                ELSE_BOP_CASE(>=)
                break;
            case ASTNodeType::Multiplicative:
                BOP_CASE(*)
                else if (node->value_ == "/")
                {
                    // 除法运算，如果除数为0，抛出异常
                    if (evaluate(node->children_[1]).value == 0)
                    {
                        throw std::runtime_error("division by zero.");
                    }
                    return {evaluate(node->children_[0]).value /
                            evaluate(node->children_[1]).value};
                }
                break;
            case ASTNodeType::Additive:
                BOP_CASE(+)
                ELSE_BOP_CASE(-)
                break;
            case ASTNodeType::IntLiteral:
                return {std::stoi(node->value_)};
            case ASTNodeType::Identifier:
                if (variables_.find(node->value_) != variables_.end())
                {
                    return {variables_[node->value_], node->value_};
                }
                throw std::runtime_error("variable '" + node->value_ +
                                         "' is not defined.");
        }
        std::cerr << "不可能走到这，应该是缺少了case语句" << std::endl;
        return {0};
    }

#undef BOP_CASE
#undef ELSE_BOP_CASE

  private:
    std::unordered_map<std::string, int> variables_;
    std::vector<EvaluatorResult> results_;  // 赋值语句的结果
    bool verbose_{false};
};
