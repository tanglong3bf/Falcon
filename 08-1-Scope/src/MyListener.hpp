#pragma once

#include "./generated/FalconScriptBaseListener.h"
#include "AnnotatedTree.hpp"
#include "Scope.hpp"

class MyListener : public FalconScriptBaseListener
{
  public:
    MyListener(AnnotatedTree* at) : at_(at)
    {
    }

  public:
    virtual void enterProg(FalconScriptParser::ProgContext* ctx) override
    {
        auto blockScope = std::make_shared<BlockScope>(nullptr, ctx, "global");

        at_->ast = ctx;
        at_->node2scope[ctx] = blockScope.get();
        scopeStack_.push(blockScope);
    }

    virtual void exitProg(FalconScriptParser::ProgContext* /*ctx*/) override
    {
        scopeStack_.pop();
    }

    virtual void enterBlock(FalconScriptParser::BlockContext* ctx) override
    {
        // TODO: 检查父节点是否是函数
        auto blockScope = std::make_shared<BlockScope>(scopeStack_.top().get(), ctx);
        at_->node2scope[ctx] = blockScope.get();
        scopeStack_.push(blockScope);
    }

    virtual void exitBlock(FalconScriptParser::BlockContext* /*ctx*/) override
    {
        // TODO: 检查父节点是否是函数
        scopeStack_.pop();
    }

    virtual void enterStatement(
        FalconScriptParser::StatementContext* ctx) override
    {
        // for 循环的 init 部分可能会定义变量
        if (ctx->FOR())
        {
            auto blockScope =
                std::make_shared<BlockScope>(scopeStack_.top().get(), ctx);
            at_->node2scope[ctx] = blockScope.get();
            scopeStack_.push(blockScope);
        }
    }

    virtual void exitStatement(
        FalconScriptParser::StatementContext* ctx) override
    {
        if (ctx->FOR())
        {
            scopeStack_.pop();
        }
    }

  private:
    AnnotatedTree* at_;
    std::stack<std::shared_ptr<Scope>> scopeStack_;
};
