#pragma once

#include "Scope.hpp"
#include <sstream>

class StackFrame
{
  public:
    StackFrame(BlockScope* scope) : scope_(scope), parentFrame(nullptr)
    {
    }

    ~StackFrame()
    {
    }

    antlrcpp::Any getVariable(const std::string& name,
                              bool checkParent = true) const
    {
        if (variables_.find(name) != variables_.end())
        {
            return variables_.at(name);
        }
        else if (checkParent && parentFrame != nullptr)
        {
            return parentFrame->getVariable(name);
        }
        else
        {
            return nullptr;
        }
    }

    void addVariable(const std::string& name, int* value)
    {
        if (getVariable(name, false).isNotNull())
        {
            std::stringstream ss;
            ss << "variable " << name << " already exists in this scope."
               << std::endl;
            throw std::runtime_error(ss.str());
        }
        variables_[name] = value;
    }

  public:
    Scope* getScope() const
    {
        return scope_;
    }

  public:
    StackFrame* parentFrame;

  private:
    Scope* scope_;
    std::unordered_map<std::string, antlrcpp::Any> variables_;
};
