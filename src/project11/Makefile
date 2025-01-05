CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic
SRC = src/main.c src/tokenizer.c src/compile_engine.c src/symbol_table.c src/vm_writer.c
EXE = bin/Compiler

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

run: $(EXE)
	$(EXE) ./provied_test/Square

.PHONY: clean
clean:
	rm -f $(EXE)
