# All Targets
all: sic

# Tool invocations
# Executable "main" depends on the files...

sic: sic.o
	gcc -m64 -g -Wall -L\usr\lib sic.o -o sic

# Depends on the source and header files
sic.o: sic.s
	nasm -g -f elf64 -w+all sic.s -o sic.o
	
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o sic
