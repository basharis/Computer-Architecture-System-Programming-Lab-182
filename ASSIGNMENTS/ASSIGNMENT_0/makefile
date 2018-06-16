
all: ass0


ass0: main.o asm.o
	gcc -g -Wall -o ass0 main.o asm.o

main.o: main.c
	gcc -g -Wall -c -o main.o main.c 

asm.o: asm.s
	nasm -g -f elf64 -w+all -o asm.o asm.s


.PHONY: clean

clean: 
	rm -f *.o ass0