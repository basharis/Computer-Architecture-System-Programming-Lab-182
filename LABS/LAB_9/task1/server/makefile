PROG = server

CC = gcc
LD = gcc

LDFLAGS = -g -Wall
CCFLAGS = -g -Wall -c -m64

OBJS = server.o line_parser.o common.o

# All Targets
all: $(PROG)

# Tool invocations
# Executable "main" depends on the files...

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

# Depends on the source and header files

server.o: server.c server.h
	$(CC) $(CCFLAGS) server.c -o server.o

line_parser.o: line_parser.c line_parser.h
	$(CC) $(CCFLAGS) line_parser.c -o line_parser.o

common.o: common.h common.c
	$(CC) $(CCFLAGS) common.c -o common.o
	
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o $(PROG)
