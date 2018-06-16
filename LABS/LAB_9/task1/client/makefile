PROG = client

CC = gcc
LD = gcc

LDFLAGS = -g -Wall
CCFLAGS = -g -Wall -c -m64

OBJS = client.o line_parser.o common.o

# All Targets
all: $(PROG)

# Tool invocations
# Executable "main" depends on the files...

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

# Depends on the source and header files

client.o: client.c client.h
	$(CC) $(CCFLAGS) client.c -o client.o

line_parser.o: line_parser.c line_parser.h
	$(CC) $(CCFLAGS) line_parser.c -o line_parser.o

common.o: common.h common.c
	$(CC) $(CCFLAGS) common.c -o common.o
	
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o $(PROG)
