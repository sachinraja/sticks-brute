GCC=g++
CFLAGS=-std=c++17 -Wall -Wextra -Werror

main: main.cpp
	$(GCC) $(CFLAGS) -o main main.cpp

.PHONY: clean

clean:
	rm -f main
