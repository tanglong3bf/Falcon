#pragma once

#include <cstring>
#include <memory>
#include <vector>
#include <iostream>

enum class ASTNodeType
{
    IntDeclaration,  ///< 整数变量声明或定义
    Assignment,      ///< 赋值语句
    Logical,         ///< 逻辑运算
    Relational,      ///< 关系运算
    Multiplicative,  ///< 乘法运算
    Additive,        ///< 加法运算
    IntLiteral,      ///< 整数字面量
    Identifier,      ///< 标识符
};

// 这种修改后的不会报警告了
#define PRINT_AST_NODE_TYPE(type)                     \
    case type:                                        \
    {                                                 \
        static const char* fullStr = #type;           \
        os << fullStr + std::strlen("ASTNodeType::"); \
        break;                                        \
    }

/**
 * @brief 输出AST节点类型
 */
inline std::ostream& operator<<(std::ostream& os, const ASTNodeType& type)
{
    switch (type)
    {
        PRINT_AST_NODE_TYPE(ASTNodeType::IntDeclaration)
        PRINT_AST_NODE_TYPE(ASTNodeType::Assignment)
        PRINT_AST_NODE_TYPE(ASTNodeType::Logical)
        PRINT_AST_NODE_TYPE(ASTNodeType::Relational)
        PRINT_AST_NODE_TYPE(ASTNodeType::Multiplicative)
        PRINT_AST_NODE_TYPE(ASTNodeType::Additive)
        PRINT_AST_NODE_TYPE(ASTNodeType::IntLiteral)
        PRINT_AST_NODE_TYPE(ASTNodeType::Identifier)
    }
    return os;
}

#undef PRINT_AST_NODE_TYPE

class ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;

/**
 * @brief 抽象语法树节点
 *
 * 包含了节点类型、值、子节点、父节点。
 */
class ASTNode : public std::enable_shared_from_this<ASTNode>
{
  public:
    virtual ~ASTNode() = default;

  public:
    ASTNode(ASTNodeType type, const std::string& value = "")
        : children_{}, type_(type), value_(value)
    {
    }

    void addChild(ASTNodePtr child)
    {
        children_.push_back(child);
        child->setParent(shared_from_this());
    }

    void clearChildren()
    {
        children_.clear();
    }

    void setParent(ASTNodePtr parent)
    {
        parent_ = parent;
    }

    ASTNodePtr getParent() const
    {
        return parent_;
    }

    const std::vector<ASTNodePtr>& getChildren() const
    {
        return children_;
    }

    ASTNodeType getType() const
    {
        return type_;
    }

    const std::string& getValue() const
    {
        return value_;
    }

    /**
     * @brief 打印抽象语法树节点
     */
    void print(std::vector<int> indent = {}) const
    {
        auto generateIndent =
            [](const std::vector<int>& indent) -> std::string {
            std::string result;
            for (size_t i = 0; i < indent.size(); ++i)
            {
                result += i + 1 == indent.size()
                              ? (indent[i] ? "├── " : "└── ")
                              : (indent[i] ? "│   " : "    ");
            }
            return result;
        };
        std::cout << generateIndent(indent) << "[" << type_ << "]";
        if (!value_.empty())
        {
            std::cout << " (" << value_ << ")";
        }
        std::cout << std::endl;
        if (children_.size() > 0)
        {
            indent.emplace_back(children_.size() > 1 ? 1 : 0);
            for (size_t i = 0; i < children_.size(); ++i)
            {
                if (i + 1 == children_.size())
                {
                    indent.back() = 0;
                }
                children_[i]->print(indent);
            }
        }
    }

#define BOP_CASE(op)                                                 \
    if (value_ == #op)                                               \
    {                                                                \
        return children_[0]->evaluate() op children_[1]->evaluate(); \
    }
#define ELSE_BOP_CASE(op) else BOP_CASE(op)

    /**
     * @brief 计算抽象语法树节点的值
     */
    int evaluate() const
    {
        switch (type_)
        {
            case ASTNodeType::IntDeclaration:
                return children_[0]->evaluate();

            case ASTNodeType::Assignment:  // 修改左侧变量的值，放到下一节再做
                return children_[1]->evaluate();
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
                ELSE_BOP_CASE(/)
                break;
            case ASTNodeType::Additive:
                BOP_CASE(+)
                ELSE_BOP_CASE(-)
                break;
            case ASTNodeType::IntLiteral:
                return std::stoi(value_);
            case ASTNodeType::Identifier:
                return 0;  // 应该读取变量的值，放到下一节再做
        }
        std::cerr << "不可能走到这，应该是缺少了case语句" << std::endl;
        return 0;
    }

#undef BOP_CASE

  private:
    ASTNodePtr parent_;                 ///< 父节点
    std::vector<ASTNodePtr> children_;  ///< 子节点列表
    ASTNodeType type_;                  ///< 节点类型
    std::string value_;                 ///< 节点内容
};
