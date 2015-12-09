CC         = gcc
MAIN_EXEC  = dg_uffsdb
DEBUG_TOOL = valgrind

# Main compilation rules
all: prog
withyacc: yacc prog # Also 'compiles' LEX and YaCC

prog: *.c interface/*.c interface/*.h
	$(CC) *.c interface/*.c -lfl -pthread -o $(MAIN_EXEC) -Wall -g

yacc: interface/*.y interface/*.l
	cd interface && make

run: withyacc
	./$(MAIN_EXEC)

debug: withyacc
	$(DEBUG_TOOL) ./$(MAIN_EXEC)

clean:
	rm $(MAIN_EXEC)
	rm data/ -R
