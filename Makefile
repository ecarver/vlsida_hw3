CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++0x

default: complement_computation

complement_computation: complement_computation.cpp parse_cover.hpp
	$(CC) $(CFLAGS) -O3 complement_computation.cpp -o cc

cc_debug: complement_computation.cpp parse_cover.hpp
	$(CC) $(CFLAGS) -O0 -DDEBUG complement_computation.cpp -o cc

clean:
	rm -f tc cc
