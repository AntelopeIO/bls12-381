all: test

test: Makefile arithmetic.cpp fp.cpp g.cpp pairing.cpp arithmetic.hpp fp.hpp g.hpp pairing.hpp scalar.hpp scalar.cpp signatures.hpp signatures.cpp bls12_381.hpp sha256.hpp sha256.cpp test.cpp
	g++ -std=c++20 -g -O0 -Wall -o test arithmetic.cpp fp.cpp g.cpp pairing.cpp sha256.cpp scalar.cpp signatures.cpp test.cpp

clean:
	rm -rf test