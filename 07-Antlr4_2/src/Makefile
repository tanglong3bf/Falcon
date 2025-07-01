# 编译器
CXX = g++
# 编译选项
CXXFLAGS = -I/usr/local/include/antlr4-runtime/ -g -O0
# 链接选项
LDFLAGS = -L/usr/local/lib/ -lantlr4-runtime

# antlr4生成的中间文件目录
GEN_DIR = generated
# 所有的.o文件目录
OBJ_DIR = objs

# .o文件名列表
OBJ_NAMES = FalconScriptBaseListener.o\
		  FalconScriptBaseVisitor.o FalconScriptLexer.o FalconScriptListener.o\
		  FalconScriptParser.o FalconScriptVisitor.o main.o 

# 拼接了路径的实际.o文件列表
OBJ_FILES = $(addprefix $(GEN_DIR)/$(OBJ_DIR)/, $(OBJ_NAMES))

# antlr4生成的中间文件列表
MIDDLE_FILES = $(addprefix $(GEN_DIR)/, $(OBJ_NAMES:.o=.cpp) $(OBJ_NAMES:.o=.h)\
	FalconScript.interp FalconScript.tokens FalconScriptLexer.interp\
	FalconScriptLexer.tokens)

# 自动创建目录
$(shell mkdir -p $(GEN_DIR)/$(OBJ_DIR))

# 最终目标
falcon: $(OBJ_FILES)
	$(CXX) $^ $(LDFLAGS) -o $@

# .o文件生成规则
$(GEN_DIR)/$(OBJ_DIR)/%.o: $(GEN_DIR)/%.cpp $(GEN_DIR)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# main.o特殊处理
$(GEN_DIR)/$(OBJ_DIR)/main.o: main.cc $(GEN_DIR)/FalconScriptLexer.h\
	$(GEN_DIR)/FalconScriptParser.h MyVisitor.hpp FalconAny.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# antlr4生成规则
$(MIDDLE_FILES): FalconScript.g4 FalconLexer.g4
	antlr4 $< -Dlanguage=Cpp -visitor -o $(GEN_DIR)

.PHONY: clean
clean:
	-rm -f falcon
	-rm -rf $(GEN_DIR)
