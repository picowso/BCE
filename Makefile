all:
	clear
	g++ -O3 main.cpp helper.cpp engine.cpp header.hpp -std=c++20 -o briwat
	./chess

uci:
	clear
	g++ -O3 main_uci.cpp helper.cpp engine.cpp header.hpp -std=c++20 -o briwat

clean:
	rm *gch *pch chess