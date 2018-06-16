#include "toy_stdio.h"

int main(int argc, char *argv[]) {
	/*
	int chars_printed = toy_printf("%x, %X\\n", 496351, 496351);
	chars_printed += toy_printf("Welcome to \\c\\n");
	chars_printed += toy_printf("Support for explicit\\N");
	toy_printf("Printed %d chars\n", chars_printed);
	*/
        
        //task 1a
        int chars_printed = toy_printf("hex unsigned: %x\n", -1);
        chars_printed += toy_printf("HEX Positive: %X\n", 268);
        chars_printed += toy_printf("Octal unsigned: %o\n", -1);
        chars_printed += toy_printf("Decimal signed: %d\n", -1);
	toy_printf("Printed %d chars\n", chars_printed);
        
        //task 1b
        chars_printed = 0;
        chars_printed = toy_printf("Unsigned value: %u\n", 15);
	chars_printed += toy_printf("Unsigned value: %u\n", -15);
	toy_printf("Printed %d chars\n", chars_printed);
        
        // task 1c
        int integers_array[] = { 1,2,3,4,5 };
	char * strings_array[] = { "This", "is", "array", "of", "strings" };
	int hex_array[] = { 10, 11, 12, 13, 14 };
	unsigned int uint_array[] = { -1, -2, -3, -4, -5 };
	int array_size = 5;
	toy_printf("Print array of integers: %Ad\n", integers_array, array_size);
	toy_printf("Print array of strings: %As\n", strings_array, array_size);
	toy_printf("Print array of hex integers: %Ax\n", hex_array, array_size);
	toy_printf("Print array of HEX integers: %AX\nand another int: %d\nand an array of unsigned ints: %Au\n", hex_array, array_size, 9, uint_array, array_size);
        
        // task 1d
	toy_printf("Non-padded string: %s\n", "str");
	toy_printf("Right-padded int: %10d\n", 6);
	toy_printf("Right-padded string: %10s\n", "str");
	toy_printf("Left-padded string: %-10s\n", "str");
	toy_printf("With numeric placeholders: %011d\n", -1234);
	
}