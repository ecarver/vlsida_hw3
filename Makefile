CC = g++
CFLAGS = -g -Wall -Wextra -pedantic -std=c++0x

default: complement_computation tautology

complement_computation: complement_computation.cpp parse_cover.hpp
	$(CC) $(CFLAGS) -O3 complement_computation.cpp -o cc

cc_debug: complement_computation.cpp parse_cover.hpp
	$(CC) $(CFLAGS) -O0 -g -DDEBUG complement_computation.cpp -o cc

tautology: tautology_check.cpp
	$(CC) $(CFLAGS) -O3 tautology_check.cpp -o tc

clean:
	rm -f tc cc
