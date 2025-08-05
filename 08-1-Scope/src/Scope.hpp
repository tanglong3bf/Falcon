#pragma once

#include <support/Any.h>

class Scope;

/**
 * 符号
 *
 * 比如函数、类
 */
class Symbol
{
  public:
    virtual ~Symbol()
    {
    }

    const std::string& getName() const
    {
        return name_;
    }

    Scope* getEnclosingScope() const
    {
        return enclosingScope_;
    }

  protected:
    std::string name_;       ///< 符号的名字，比如函数名、类名
    Scope* enclosingScope_;  ///< 所属作用域
    antlr4::ParserRuleContext* ctx_;  ///< 对应的AST节点
};

/**
 * 作用域
 *
 * 包括全局作用域、类作用域、函数作用域、块作用域
 */
class Scope : public Symbol
{
  public:
    Scope()
    {
    }

    virtual ~Scope()
    {
    }

  protected:
    std::vector<Symbol> symbols_;  ///< 当前作用域中的符号表
    friend class MyListener;
};

/**
 * 块作用域
 *
 * 用于表示代码块，比如if语句、for语句等
 */
class BlockScope : public Scope
{
  public:
    BlockScope(Scope* enclosingScope,
               antlr4::ParserRuleContext* ctx,
               const std::string& name = "")
    {
        enclosingScope_ = enclosingScope;
        ctx_ = ctx;
        if (name != "")
        {
            name_ = "block_" + name;
        }
        else
        {
            name_ = "block_" + std::to_string(block_id++);
        }
    }

    virtual ~BlockScope()
    {
    }

  private:
    static uint32_t block_id;
};

inline uint32_t BlockScope::block_id = 0;
