all:
	g++ -g -O3 -march=native main.cpp board.cpp engine.cpp header.hpp -std=c++20 -o briwat

uci:
	g++ -fsanitize=address,undefined,leak -g main_uci.cpp board.cpp engine.cpp header.hpp -std=c++20 -o briwat

clean:
	find . -path './.git' -prune -o -type f ! \( -name '*.hpp' -o -name '*.cpp' -o -name '*.md' -o -name 'Makefile' \) -exec rm -f {} +
