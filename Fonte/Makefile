all: prog

prog: *.c interface/*.c interface/*.h
	gcc *.c interface/*.c -lfl -pthread -o Ibetres -Wall -g

withyacc: yacc prog

yacc: interface/*.y interface/*.l
	cd interface && make

clean:
	rm Ibetres
	rm data/ -R
