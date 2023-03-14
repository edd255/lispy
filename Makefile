#==== DEFINITIONS ==============================================================
#---- TARGET -------------------------------------------------------------------

NAME    := clisp
VERSION := 0.1

#---- TOOLS --------------------------------------------------------------------

CC := clang
LD := clang
RM := rm --force

#---- DIRS ---------------------------------------------------------------------

SRC_DIRS := ./src
LIB_DIRS := ./deps
BIN_DIR  := ./bin

#---- FILES --------------------------------------------------------------------

LIBS := $(shell find $(LIB_DIRS) -name *.c)
SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
BIN  := ${BIN_DIR}/${NAME}

#---- FLAGS --------------------------------------------------------------------

ERROR_FLAGS    := -Wall -Wpedantic -Wextra -Werror
DEBUG_FLAGS    := -Og -g -fsanitize=address -fsanitize=pointer-compare \
				  -fsanitize=pointer-subtract -fsanitize=shadow-call-stack \
				  -fsanitize=leak -fsanitize=undefined \
				  -fsanitize-address-use-after-scope
OPT_FLAGS      := -Ofast -DNDEBUG
LD_FLAGS       := -ledit
RELEASE_FLAGS  := ${ERROR_FLAGS} ${OPT_FLAGS} ${LD_FLAGS}
TESTING_FLAGS  := ${ERROR_FLAGS} ${DEBUG_FLAGS} ${LD_FLAGS}
VALGRIND_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
MAKEFLAGS      := --jobs=$(shell nproc)

#==== PROCEDURES ===============================================================

all: opt debug

opt:
	${CC} ${RELEASE_FLAGS} ${SRCS} ${LIBS} -o ${BIN}_opt

debug:
	export detect_invalid_pointer_pairs=2
	${CC} ${TESTING_FLAGS} ${SRCS} ${LIBS} -o ${BIN}_debug

clean:
	${RM} ${BIN}_opt ${BIN}_debug 

memcheck:
	valgrind ${VALGRIND_FLAGS} ./${BIN}_debug
