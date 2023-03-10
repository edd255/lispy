CC      := clang
LIB     := deps/mpc/mpc.c
HEADERS := src/eval.c src/lisp_value.c
MAIN    := src/main.c
OUT     := bin/clisp

ERROR_FLAGS    := -Wall -Wpedantic -Wextra -Werror
DEBUG_FLAGS    := -Og -g -fsanitize=address -fsanitize=undefined
OPT_FLAGS      := -Ofast -DNDEBUG
LD_FLAGS       := -ledit -lm
VALGRIND_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
MAKEFLAGS      := --jobs=$(shell nproc)

all: debug opt

debug:
	${CC} ${DEBUG_FLAGS} ${ERROR_FLAGS} ${MAIN} ${LIB} ${HEADERS} ${LD_FLAGS} -o ${OUT}_debug

opt:
	${CC} ${OPT_FLAGS} ${ERROR_FLAGS} ${MAIN} ${LIB} ${HEADERS} ${LD_FLAGS} -o ${OUT}_opt

memcheck: 
	valgrind ${VALGRIND_FLAGS} ./${OUT}_debug
