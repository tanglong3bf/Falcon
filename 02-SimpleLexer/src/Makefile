.PHONY: clean run
run: app
	./app
clean:
	rm ./app -rf

app: main.cc lexer.hpp token.hpp
	g++ $< -o $@

