#include "toy_stdio.h"

int main(int argc, char *argv[]) {
    int i;
    i = toy_printf("%x, %x\n",1, -1);
    toy_printf("%d\n", i);
    i = toy_printf("%03d, %10s\n", -1, "hello");
    toy_printf("%d\n", i);
    int iArray[] = {1, 2, 3};
    char cArray[] = {'a', 'b', 'c'};
    char* sArray[] = {"A", "B", "C"};
    i = toy_printf("%Ad\n%Ac\n%As\n", iArray, 3, cArray, 3, sArray, 3);
    toy_printf("%d\n", i);
}
/*int main(int argc, char *argv[]) {
	toy_printf("\n##### TASK 1a #####\n");
	int chars_printed = 0;
	chars_printed = toy_printf("hex unsigned: %x\n", -1);
	chars_printed += toy_printf("HEX Positive: %X\n", 268);
	chars_printed += toy_printf("Octal unsigned: %o\n", -1);
	chars_printed += toy_printf("Decimal signed: %d\n", -1);
	toy_printf("Printed %d chars\n", chars_printed);

	toy_printf("\n##### TASK 1b #####\n");
	chars_printed = 0;
	chars_printed = toy_printf("Unsigned value: %u\n", 15);
	chars_printed += toy_printf("Unsigned value: %u\n", -15);
	toy_printf("Printed %d chars\n", chars_printed);

	toy_printf("\n##### TASK 1c #####\n");
	int integers_array[] = { 1,2,3,4,5 };
	char * strings_array[] = { "This", "is", "array", "of", "strings" };
	int hex_array[] = { 10, 11, 12, 13, 14 };
	unsigned int uint_array[] = { -1, -2, -3, -4, -5 };
	char char_array[] = { 'a', 'b', 'c' };
	int array_size = 5;
	toy_printf("Print array of integers: %Ad\n", integers_array, array_size);
	toy_printf("Print array of strings: %As\n", strings_array, array_size);
	toy_printf("Print array of hex integers: %Ax\n", hex_array, array_size);
	toy_printf("Print array of HEX integers: %AX\nand another int: %d\nand an array of unsigned ints: %Au\n", hex_array, array_size, 9, uint_array, array_size);
	toy_printf("Print smaller array of chars: %Ac\n", char_array, 3);

	toy_printf("\n##### TASK 1d #####\n");
	toy_printf("%015d\n", -199);
	toy_printf("%20s\n", "Hello");
	toy_printf("%3d, %10s, %-8d\n", 100, "Hello", 100);
}*/