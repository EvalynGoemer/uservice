all:
	gcc -O2 -Wall -Wextra -Wpedantic -static -o uservice uservice.c
debug:
	gcc -O0 -Wall -Wextra -Wpedantic -static -DDEBUG -o uservice uservice.c
