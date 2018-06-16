
all: toy_printf

toy_printf: main.o toy_printf.o
	gcc -g -Wall -o toy_printf main.o toy_printf.o

main.o: main.c toy_stdio.h
	gcc -g -Wall -c -o main.o main.c

toy_printf.o: toy_printf.c
	gcc -g -Wall -c -o toy_printf.o toy_printf.c

.PHONY: clean

clean: 
	rm -f *.o toy_printf