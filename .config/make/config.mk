#==== DEFINITIONS ==============================================================
#---- TARGET -------------------------------------------------------------------

NAME    := lispy
VERSION := 0.1

#---- TOOLS --------------------------------------------------------------------

CC     := ccache clang
LD     := ccache clang -fuse-ld=mold
RM     := rm
MKDIR  := mkdir --parents
Q      ?= @

#---- INSTALL DIRECTORIES ------------------------------------------------------

PREFIX := /usr/local

#---- CODE DIRECTORIES ---------------------------------------------------------

SRC_DIR   := src
BUILD_DIR := build
BIN_DIR   := bin
LIB       := lib

#---- CODE FILES ---------------------------------------------------------------

PROJ_SRCS := $(shell find src -type f -name "*.c" -o -name "*.h")
SRC_DIRS  := $(SRC_DIR) $(wildcard ./deps/*)
INC_DIRS  := $(shell find $(SRC_DIRS) -type d)
TESTS     := $(wildcard tests/*)

#---- OBJECTS, BINARIES AND DEPENDENCIES ---------------------------------------

BIN       := $(BIN_DIR)/$(NAME)
SRCS      := $(shell find $(SRC_DIRS) -name '*.c')
OPT_OBJS  := $(SRCS:%.c=$(BUILD_DIR)/%.opt.o)
PROF_OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.prof.o)
SAN_OBJS  := $(SRCS:%.c=$(BUILD_DIR)/%.san.o)
DBG_OBJS  := $(SRCS:%.c=$(BUILD_DIR)/%.dbg.o)
DEPS      := $(OBJS:.o=.d)

#---- FLAGS --------------------------------------------------------------------

INC_FLAGS         := -I. -I$(SRC_DIR)
LDFLAGS           += -ledit -lm -DLOGC_USE_COLOR -DVERSION=$(VERSION)
CFLAGS            := $(INC_FLAGS) -MMD -MP -DLOGC_USE_COLOR -DVERSION=$(VERSION)
MAKEFLAGS         := --jobs=$(shell nproc)
VALGRIND          := --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck
CALLGRIND         := --dump-instr=yes --collect-jumps=yes --tool=callgrind
MASSIF            := --tool=massif
DHAT              := --tool=dhat
CPPCHECK          := --enable=all --suppress=missingIncludeSystem $(INC_FLAGS)
CLANG_FMT_CONFIG  := --Werror --style=file:.config/clang/fmt.conf
CLANG_TIDY_CONFIG := -config-file=.config/clang/tidy.conf -export-fixes=tidy.log -fix

ERR  := -Wall \
		-Wpedantic \
		-Wextra \
		-Werror \
		-Wvla \
		-Wno-gnu-zero-variadic-macro-arguments
OPT  := -Ofast -DNDEBUG
DBG  := -Og -g
LOG  := -DLOG_ALLOCS
SAN  := -fsanitize=address \
	    -fsanitize=pointer-compare \
	    -fsanitize=pointer-subtract \
	    -fsanitize=leak \
	    -fsanitize=undefined \
	    -fsanitize-address-use-after-scope
FORT := -D_FORTIFY_SOURCE=2
PROF := -pg

RELEASE   := ${FORT} ${ERR} ${OPT}
DEBUGGING := ${FORT} ${ERR} ${DBG} ${LOG}
SANITIZED := ${FORT} ${ERR} ${DBG} ${SAN}
PROFILING := ${FORT} ${ERR} ${OPT} ${PROF}

