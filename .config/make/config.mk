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

#---- CODE DIRECTORIES ---------------------------------------------------------

PROJ_SRCS := $(shell find src -type f -name "*.c" -o -name "*.h")
SRC_DIR   := src
SRC_DIRS  := $(SRC_DIR) $(wildcard ./deps/*)
BUILD_DIR := build
BIN_DIR   := bin
INC_DIRS  := $(shell find $(SRC_DIRS) -type d)
ASSETS    := assets
TESTS     := $(wildcard tests/*)

#---- INSTALL DIRECTORIES ------------------------------------------------------

PREFIX := /usr/local

#---- FILES --------------------------------------------------------------------

BIN  := $(BIN_DIR)/$(NAME)
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

#---- FLAGS --------------------------------------------------------------------

INC_FLAGS         := -I. -I$(SRC_DIR)
LDFLAGS           += -ledit -lm -DLOGC_USE_COLOR
CFLAGS            := $(INC_FLAGS) -MMD -MP -DLOGC_USE_COLOR
MAKEFLAGS         := --jobs=$(shell nproc)
VALGRIND          := --leak-check=full --show-leak-kinds=all --track-origins=yes
CPPCHECK          := --enable=all --suppress=missingIncludeSystem $(INC_FLAGS)
CLANG_FMT_CONFIG  := --Werror --style=file:.config/clang/fmt.conf
CLANG_TIDY_CONFIG := -config-file=.config/clang/tidy.conf -export-fixes=tidy.log -fix

ERR  := -Wall -Wpedantic -Wextra -Werror -Wno-gnu-zero-variadic-macro-arguments -Wvla
OPT  := -Ofast -DNDEBUG
DBG  := -Og -g -DLOG_ALLOCS
SAN  := -fsanitize=address \
	    -fsanitize=pointer-compare \
	    -fsanitize=pointer-subtract \
	    -fsanitize=leak \
	    -fsanitize=undefined \
	    -fsanitize-address-use-after-scope
FORT := -D_FORTIFY_SOURCE=2

RELEASE   := ${FORT} ${ERR} ${OPT}
DEBUGGING := ${FORT} ${ERR} ${DBG}
SANITIZED := ${FORT} ${ERR} ${DBG} ${SAN}

