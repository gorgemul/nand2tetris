CC=gcc
CFLAGS=-std=c11
CFLAGS+=-Wall
CFLAGS+=-Wextra
CFLAGS+=-pedantic

TEST_LIB_ROOT=$(HOME)/clib/unity

SRC=$(shell find ./src -name "*.c")
SRC_WITHOUT_MAIN=$(shell find ./src -name "*.c" ! -name "main.c")

TEST_LIB=$(shell find $(TEST_LIB_ROOT) -name "*.c")
TEST_LIB_SYMBOLS=-DUNITY_FIXTURE_NO_EXTRAS
TESTS=$(shell find ./test -name "*.c")

INC_DIRS=-I$(TEST_LIB_ROOT) -Isrc

TEST_SRC=$(SRC_WITHOUT_MAIN) $(TESTS) $(TEST_LIB)

TEST_EXE=bin/test.out
EXE=bin/VMTranslator

tests:
	@$(CC) $(CFLAGS) $(INC_DIRS) $(TEST_LIB_SYMBOLS) $(TEST_SRC) -o $(TEST_EXE)
		- @./$(TEST_EXE)
		- @$(MAKE) clean

build:
	@$(CC) $(CFLAGS) -Isrc $(SRC) -o $(EXE)

clean:
	@rm bin/*
