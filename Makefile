all:
	clear
	g++ -O3 -Ofast main.cpp helper.cpp engine.cpp header.hpp -std=c++20 -o briwat
	./chess

uci:
	clear
	g++ -O3 -Ofast main_uci.cpp helper.cpp engine.cpp header.hpp -std=c++20 -o briwat

clean:
	find . -path './.git' -prune -o -type f ! \( -name '*.hpp' -o -name '*.cpp' -o -name '*.md' -o -name 'Makefile' \) -exec rm -f {} +