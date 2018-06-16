PROG = hexeditplus

CC = gcc
LD = gcc

LDFLAGS = -g -Wall
CCFLAGS = -g -Wall -c -m64

OBJS = task1.o

# All Targets
all: $(PROG)

# Tool invocations
# Executable "main" depends on the files...

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

# Depends on the source and header files

task1.o: task1.c task1.h
	$(CC) $(CCFLAGS) task1.c -o task1.o
	
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o $(PROG)
