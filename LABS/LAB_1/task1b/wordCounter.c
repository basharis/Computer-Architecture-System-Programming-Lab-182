#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	int wordsFlag = 0, charactersFlag = 0, longestFlag = 0;				// arg flags
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-w") == 0)
			wordsFlag = i;
		else if (strcmp(argv[i], "-c") == 0)
			charactersFlag = i;
		else if (strcmp(argv[i], "-l") == 0)
			longestFlag = i;
	}
	if (wordsFlag == 0 && charactersFlag == 0 && longestFlag == 0)		// default is count words
		wordsFlag = 1;
	
	char c;
	int countWords = 0;
	int countCharactersTotal = 0;
	int countCharactersCurrent = 0;
	int countCharactersLongest = 0;
	int inAWord = 0;

	while ((c = fgetc(stdin)) != EOF) {
		if (inAWord == 0) {
			if (c > ' ') {
				inAWord = 1;
				countWords++;
				countCharactersTotal++;
				countCharactersCurrent++;
			}
		}
		else {
			if (c <= ' ') {
				inAWord = 0;
				if (countCharactersCurrent > countCharactersLongest)
					countCharactersLongest = countCharactersCurrent;
				countCharactersCurrent = 0;
			}
			else {
				countCharactersTotal++;
				countCharactersCurrent++;
			}
		}
	}
	
	if (argc == 1)
		printf("total number of words: %d\n", countWords);
	else {
		for (int i = 1; i < argc; ++i)
		{
			if (wordsFlag == i)
				printf("total number of words: %d\n", countWords);
			if (charactersFlag == i)
				printf("total number of characters: %d\n", countCharactersTotal);
			if (longestFlag == i)
				printf("number of characters in the longest word: %d\n", countCharactersLongest);
		}
	}
	return 0;
}