filename=src/main.c

all:
	gcc -Wall src/main.c libs/mpc.c -ledit -lm -o clisp
