#pragma once

#include <cstring>
#include <memory>
#include <vector>
#include <iostream>

enum class ASTNodeType
{
    Program,         ///< 程序
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
        PRINT_AST_NODE_TYPE(ASTNodeType::Program)
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

    friend class Repl;

  private:
    ASTNodePtr parent_;                 ///< 父节点
    std::vector<ASTNodePtr> children_;  ///< 子节点列表
    ASTNodeType type_;                  ///< 节点类型
    std::string value_;                 ///< 节点内容
};
