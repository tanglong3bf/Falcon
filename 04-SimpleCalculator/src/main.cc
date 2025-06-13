#include <iostream>
#include "parser.hpp"

void lexerTest()
{
    // 只有!<会报错，因为单独的一个!是不合法的，它后面必须跟一个=
    std::vector<std::string> inputList = {"int d = a>b < 3 != 3 == 5;",
                                          "a+b-c*d/e >=233 <= 114514",
                                          "id <> e !<"};
    Lexer lexer;
    for (const auto& input : inputList)
    {
        lexer.setInput(input);
        std::cout << '`' << input << "`: " << std::endl;
        try
        {
            while (Token token = lexer.nextToken())
            {
                std::cout << "[" << token.type << "]: " << token.value
                          << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

void parserTest()
{
    // 暂时让第三行合法，下一节再改进
    std::vector<std::string> inputList = {
        "int a = 233 + 123 -72 * 2 / 3;",
        "int b = a / 3 < 10;",
        "a || b = c || d;",
        "a = b = c = d = 100;",
        "(2>3) + (4<5) + (6<7) == 2;"};  // true为1，false为0
    Parser parser;
    for (const auto& input : inputList)
    {
        std::cout << '`' << input << "`: " << std::endl;
        try
        {
            auto astRootNode = parser.parse(input);
            if (astRootNode)
            {
                // 打印AST
                astRootNode->print();
                // 计算节点的值
                // 如果是赋值操作，直接取右侧子节点的值
                // 如果是取变量的值，暂时用0代替
                auto result = astRootNode->evaluate();
                std::cout << "Result: " << result << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

int main()
{
    // lexerTest();
    parserTest();
    return 0;
}
