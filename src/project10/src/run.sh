#!/bin/bash

set -xe

CC="gcc"
CFLAGS="-Wall -Wextra -std=c11 -pedantic"
SRC="main.c tokenizer.c compile_engine.c"
EXE="out"

$CC $CFLAGS -o $EXE $SRC
./$EXE
