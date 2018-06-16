
all: task2


task2: wordCounter.o
	gcc -g -Wall -o wordCounter wordCounter.o

wordCount.o: wordCounter.c
	gcc -g -Wall -c -o wordCounter.o wordCounter.c 

.PHONY: clean

clean: 
	rm -f *.o wordCounter
