#!/bin/bash

set -xe

CC="gcc"
CFLAGS="-Wall -Wextra -std=c11 -pedantic"
SRC="src/main.c src/tokenizer.c src/compile_engine.c src/symbol_table.c"
EXE="bin/Parser"

$CC $CFLAGS -o $EXE $SRC
