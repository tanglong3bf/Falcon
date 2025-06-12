#include <iostream>
#include "parser.hpp"

int main()
{
    std::vector<std::string> inputList = {{"int a = 10"},
                                          {"int b = 3 + 4 * 5 + 6 + 7"},
                                          {"2+3*5"},
                                          {"-2*3*5"},
                                          {"20+*3*5"}};
    Parser parser;
    for (const auto& input : inputList)
    {
        std::cout << '`' << input << "`: " << std::endl;
        try
        {
            // 第四个和第五个都会抛异常
            auto astRootNode = parser.parse(input);
            if (astRootNode)
            {
                // 打印AST
                astRootNode->print();
                // 计算节点的值
                auto result = astRootNode->evaluate();
                std::cout << "Result: " << result << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    return 0;
}
