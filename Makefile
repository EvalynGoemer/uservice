all:
	gcc -O2 -static -o uservice uservice.c
debug:
	gcc -O0 -static -DDEBUG -o uservice uservice.c
