#include <antlr4-runtime.h>

#include "./generated/FalconScriptLexer.h"
#include "./generated/FalconScriptParser.h"
#include "MyVisitor.hpp"
#include "MyListener.hpp"

/**
 * 借助辅助栈，判断是否有未关闭的括号
 */
bool parenIsClosed(const std::string& input)
{
    std::stack<char> stack;
    for (char c : input)
    {
        if (c == '(' || c == '{' || c == '[')
        {
            stack.push(c);
        }
        else if (c == ')')
        {
            if (stack.empty() || stack.top() != '(')
            {
                return false;
            }
            stack.pop();
        }
        else if (c == '}')
        {
            if (stack.empty() || stack.top() != '{')
            {
                return false;
            }
            stack.pop();
        }
        else if (c == ']')
        {
            if (stack.empty() || stack.top() != '[')
            {
                return false;
            }
            stack.pop();
        }
    }
    return stack.empty();
}

// 从 05 的 repl 抄过来的
void repl()
{
    std::cout << "Welcome to Falcon!" << std::endl;
    std::cout << "> ";
    std::string buffer;
    std::string input;

    bool isFirstTime = true;
    AnnotatedTree at;
    MyListener listener(&at);
    // 创建自定义 visitor 实例
    MyVisitor visitor(true, &at);

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
        auto i = input.find_last_not_of(' ');
        // 检查输入是否完整
        if (input[i] == '}' || input[i] == ';')
        {
            if (!parenIsClosed(buffer))
            {
                std::cout << "> ";
                continue;
            }
        }
        // 结尾字符不是分号或右括号，输入一定不完整
        else
        {
            std::cout << "> ";
            continue;
        }
        // 输入流可以是字符串
        antlr4::ANTLRInputStream inputStream(buffer);
        // 创建词法分析器实例
        FalconScriptLexer lexer(&inputStream);
        // 解析出token
        antlr4::CommonTokenStream tokens(&lexer);
        // 创建语法分析器实例
        FalconScriptParser parser(&tokens);
        if (isFirstTime)
        {
            // 解析输入并生成解析树（AST）
            auto prog = parser.prog();
            antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, prog);

            // 遍历语法树
            visitor.visitProg(prog);
            isFirstTime = false;
        }
        else [[likely]]
        {
            parser.reset();
            auto blockStatement = parser.blockStatement();
            antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener,
                                                        blockStatement);
            // 遍历语法树
            visitor.visitBlockStatement(blockStatement);
        }

        // 清空buffer
        buffer = "";
        std::cout << "\n> ";
    }
}

void printHelp()
{
    std::cerr << "请输入： falcon [脚本文件名]" << std::endl;
}

int main(int argc, char* argv[])
{
    // repl模式
    if (argc == 1)
    {
        repl();
    }
    // 读取脚本文件
    else if (argc == 2)
    {
        std::ifstream file(argv[1]);
        if (!file.is_open())
        {
            std::cerr << "无法打开文件：" << argv[1] << std::endl;
            return 1;
        }
        // 输入流可以是文件
        antlr4::ANTLRInputStream inputStream(file);
        FalconScriptLexer lexer(&inputStream);
        antlr4::CommonTokenStream tokens(&lexer);
        FalconScriptParser parser(&tokens);
        auto* tree = parser.prog();
        // BlockScope globalScope(nullptr, tree, "global");
        AnnotatedTree at;
        MyListener listener(&at);
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
        MyVisitor visitor(false, &at);
        visitor.visitProg(tree);
    }
    else
    {
        printHelp();
    }
    return 0;
}
