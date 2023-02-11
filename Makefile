CC   := clang
LIB  := libs/mpc/mpc.c
MAIN := src/main.c
OUT  := bin/clisp

ERROR_FLAGS    := -Wall -Wpedantic -Wextra -Werror
DEBUG_FLAGS    := -Og -g -fsanitize=address -fsanitize=undefined
OPT_FLAGS      := -Ofast
LD_FLAGS       := -ledit -lm
VALGRIND_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes

all: debug opt

debug:
	${CC} ${DEBUG_FLAGS} ${ERROR_FLAGS} ${MAIN} ${LIB} ${LD_FLAGS} -o ${OUT}_debug

opt:
	${CC} ${OPT_FLAGS} ${ERROR_FLAGS} ${MAIN} ${LIB} ${LD_FLAGS} -o ${OUT}_opt

run: 
	valgrind ${VALGRIND_FLAGS} ./${OUT}_debug
