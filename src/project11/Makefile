CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic
SRC = src/main.c src/tokenizer.c src/compile_engine.c src/symbol_table.c src/vm_writer.c
EXE = bin/Parser

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(EXE)
