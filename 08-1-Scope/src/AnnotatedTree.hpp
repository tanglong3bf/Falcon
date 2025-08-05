#pragma once

#include "Scope.hpp"
#include <support/Declarations.h>
#include <tree/ParseTree.h>

class AnnotatedTree
{
  public:
    AnnotatedTree()
    {
    }

    ~AnnotatedTree()
    {
    }

  public:
    antlr4::tree::ParseTree* ast;
    std::unordered_map<antlr4::ParserRuleContext*, Scope*> node2scope;
};
