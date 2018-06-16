#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	int wordsFlag = 0, charactersFlag = 0, longestFlag = 0, fileFlag = 0, linesFlag = 0; // arg flags
	char* fileName;
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-w") == 0)
			wordsFlag = i;
		else if (strcmp(argv[i], "-c") == 0)
			charactersFlag = i;
		else if (strcmp(argv[i], "-l") == 0)
			longestFlag = i;
		else if (strcmp(argv[i], "-n") == 0)
			linesFlag = i;
		else if (strcmp(argv[i], "-i") == 0) {
			fileFlag = i;
			fileName = argv[i + 1];
		}
	}
	if (wordsFlag == 0 && charactersFlag == 0 && longestFlag == 0 && linesFlag == 0)		// default is count words
		wordsFlag = 1;

	char c;
	int countWords = 0;
	int countCharactersTotal = 0;
	int countCharactersCurrent = 0;
	int countCharactersLongest = 0;
	int countLines = 0;
	int inAWord = 0;
	
	FILE* stream;
	if (fileFlag == 0)
		stream = stdin;
	else
		stream = fopen(fileName, "r");

	while ((c = fgetc(stream)) != EOF) {
		if (inAWord == 0) {
			if (c > ' ') {
				inAWord = 1;
				countWords++;
				countCharactersTotal++;
				countCharactersCurrent++;
			}
			else {
				if (c == '\n')
					countLines++;
			}
		}
		else {
			if (c <= ' ') {
				inAWord = 0;
				if (countCharactersCurrent > countCharactersLongest)
					countCharactersLongest = countCharactersCurrent;
				countCharactersCurrent = 0;
				if (c == '\n')
					countLines++;
			}
			else {
				countCharactersTotal++;
				countCharactersCurrent++;
			}
		}
	}


	if (argc == 1 || (argc == 3 && wordsFlag != 0))
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
			if (linesFlag == i)
				printf("total number of lines: %d\n", countLines);
		}
	}

	if (fileFlag > 0) {
		fclose(stream);
	}
	return 0;
}