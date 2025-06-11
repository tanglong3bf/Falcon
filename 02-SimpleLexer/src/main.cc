#include <iostream>
#include <vector>
#include "./lexer.hpp"

int main()
{
    // 注意：这里只考虑了词法上是否合法，并未考虑语法上是否合法
    // 连续的`=`会被解析为多个赋值号，不报错
    // `in` 和 `intA` 会被理解成标识符，不报错（虽然所在语句有语法错误）
    // 不认识分号`;`，会报错
    std::vector<std::string> inputList = {"int a =    10",
                                          "intA === a>3",
                                          "in b = 233",
                                          "int c = a + b;"};
    Lexer lexer;
    for (const auto& input : inputList)
    {
        lexer.setInput(input);
        std::cout << '`' << input << "`: " << std::endl;
        while (Token token = lexer.nextToken())
        {
            std::cout << token.type << " " << token.value << std::endl;
        }
    }
    return 0;
}
