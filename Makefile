compile:
	g++ -O3 main.cpp nnue.cpp board.cpp engine.cpp header.hpp -std=c++20 -o briwat

clean:
	find . -path './.git' -prune -o -type f ! \( -name '*.hpp' -o -name '*.cpp' -o -name '*.md' -o -name 'Makefile' \) -exec rm -f {} +

debug:
	g++ -g main.cpp nnue.cpp board.cpp engine.cpp header.hpp -std=c++20 -o briwat