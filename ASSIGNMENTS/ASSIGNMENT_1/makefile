CC = gcc
CFLAGS  = -m64 -g -Wall
LFLAGS  = -m64 -g -Wall -L\usr\lib

# All Targets
all: calc

# Tool invocations
# Executable "main" depends on the files...

calc: main.o ops.o
	@echo 'Building target: main'
	@echo 'Invoking: C Linker'
	$(CC) -o calc main.o ops.o $(LFLAGS)
	@echo 'Finished building target: main'
	@echo ' '

# Depends on the source and header files
main.o: main.c
	$(CC) $(CFLAGS) -c  main.c
	
ops.o: ops.s
	nasm -g -f elf64 -w+all -o ops.o ops.s
	
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o calc

