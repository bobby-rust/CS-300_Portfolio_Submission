COMPILER=g++
OPTIONS=-g -std=c++17 -pedantic -Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code
COMPILE=$(COMPILER) $(OPTIONS)

build/main: main.cpp dag.h course.h
	$(COMPILE) -o $@ $^

clean:
	rm main

.PHONY: main

