all:
	g++ fsanitize=address,undefined,leak main.cpp board.cpp engine.cpp header.hpp -std=c++20 -o briwat

uci:
	g++ -g3 -O0 -Wall -Wextra -pedantic -fsanitize=address main_uci.cpp board.cpp engine.cpp header.hpp -std=c++20 -o briwat

clean:
	find . -path './.git' -prune -o -type f ! \( -name '*.hpp' -o -name '*.cpp' -o -name '*.md' -o -name 'Makefile' \) -exec rm -f {} +
