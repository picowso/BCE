all:
	g++ fsanitize=address,undefined,leak main.cpp board.cpp engine.cpp header.hpp -std=c++20 -o briwat

uci:
	g++ -O3 -march=native -flto -pipe -std=c++17 -DNDEBUG -funroll-loops -fomit-frame-pointer main_uci.cpp board.cpp engine.cpp header.hpp -std=c++20 -o briwat

clean:
	find . -path './.git' -prune -o -type f ! \( -name '*.hpp' -o -name '*.cpp' -o -name '*.md' -o -name 'Makefile' \) -exec rm -f {} +
