#==== DEFINITIONS ==============================================================
#---- TARGET -------------------------------------------------------------------

NAME    := lispy
VERSION := 0.1

#---- TOOLS --------------------------------------------------------------------

CCACHE := ccache
CC     := $(CCACHE) clang
LD     := $(CCACHE) clang
RM     := rm
MKDIR  := mkdir --parents
Q      ?= @

#---- CODE DIRECTORIES ---------------------------------------------------------

SRC_DIR   := src
SRC_DIRS  := $(SRC_DIR) $(wildcard ./deps/*)
BUILD_DIR := build
BIN_DIR   := bin
INC_DIRS  := $(shell find $(SRC_DIRS) -type d)
ASSETS    := assets

#---- INSTALL DIRECTORIES ------------------------------------------------------

PREFIX := /usr/local

#---- FILES --------------------------------------------------------------------

BIN  := $(BIN_DIR)/$(NAME)
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

#---- FLAGS --------------------------------------------------------------------

LDFLAGS   += -ledit -lm
CFLAGS    := $(INC_FLAGS) -MMD -MP
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
MAKEFLAGS := --jobs=$(shell nproc)

ERR := -Wall -Wpedantic -Wextra -Werror -Wno-gnu-zero-variadic-macro-arguments
OPT := -Ofast -DNDEBUG
DBG := -Og -g 
SAN := -fsanitize=address \
	   -fsanitize=pointer-compare \
	   -fsanitize=pointer-subtract \
	   -fsanitize=shadow-call-stack \
	   -fsanitize=leak \
	   -fsanitize=undefined \
	   -fsanitize-address-use-after-scope

RELEASE   := ${ERR} ${OPT}
DEBUGGING := ${ERR} ${DBG}
MEMCHECK  := ${ERR} ${DBG} ${SAN}

