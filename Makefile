all:
	g++ main.cpp helper.cpp engine.cpp header.hpp -std=c++20 -o chess
	./chess

clean:
	rm *gch *pch chess