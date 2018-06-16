
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* the states in the printf state-machine */
enum printf_state {
	st_printf_init,
	st_printf_meta_char,
	st_printf_percent,
	st_printf_octal2,
	st_printf_octal3
};

#define NUM_OF_STATES 5
#define TRUE 1
#define FALSE 0
#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

typedef struct state_args {
	char* fs;
	char octal_char;
} state_args;

typedef struct state_result {
	int printed_chars;
	enum printf_state next_state;
} state_result;

int is_integer_type(char c) { 
	if (c == 'd' || c == 'x' || c == 'b' || c == 'X' || c == 'o')
		return TRUE;
	return FALSE; 
}

int print_int_helper(unsigned int n, int radix, const char *digit) {
	int result;
	if (n < radix) {
		putchar(digit[n]);
		return 1;
	}
	else {
		result = print_int_helper(n / radix, radix, digit);
		putchar(digit[n % radix]);
		return 1 + result;
	}
}

int print_int(int n, int radix, const char * digit) {
	if (radix < 2 || radix > 16) {
		toy_printf("Radix must be in [2..16]: Not %d\n", radix);
		exit(-1);
	}

	if (n > 0) {
		return print_int_helper(n, radix, digit);
	}
	else if (n == 0) {
		putchar('0');
		return 1;
	}
	else {	// n < 0
		if (radix != 10) { 
			return print_int_helper(n, radix, digit);
		}
		else {
			putchar('-');
			return 1 + print_int_helper(-n, radix, digit);
		}
	}
}

int print_array_helper(void* arr, int array_size, char* fs){
	int *int_arr;
	unsigned int *uint_arr;
	char **string_arr;
	char *char_arr;
	int num_of_chars = 0;
	putchar('{');
	switch (*(++fs)) {
	case 'u':
		uint_arr = arr;
		num_of_chars += toy_printf("%u", uint_arr[0]);
		for (int i = 1; i < array_size; ++i) {
			num_of_chars += toy_printf(", %u", uint_arr[i]);
		}
		break;
	case 's':
		string_arr = arr;
		num_of_chars += toy_printf("\"%s\"", string_arr[0]);
		for (int i = 1; i < array_size; ++i) {
			num_of_chars += toy_printf(", \"%s\"", string_arr[i]);
		}
		break;
	case 'c':
		char_arr = arr;
		num_of_chars += toy_printf("\"%c\"", char_arr[0]);
		for (int i = 1; i < array_size; ++i) {
			num_of_chars += toy_printf(", \"%c\"", char_arr[i]);
		}
		break;
	default:
		// d, x, b, X, o
		if (is_integer_type(*fs)) {
			char print_instruction[3] = { '%', *fs, '\0' };
			int_arr = arr;
			num_of_chars += toy_printf(print_instruction, int_arr[0]);
			for (int i = 1; i < array_size; ++i) {
				num_of_chars += toy_printf(", ");
				num_of_chars += toy_printf(print_instruction, int_arr[i]);
			}
		}
	}
	putchar('}');
	num_of_chars += 2; // {}
	return num_of_chars;
}

int count_int_digits(int int_to_pad) {
	int count = 1;
	while (int_to_pad != 0)
	{
		int_to_pad /= 10;
		++count;
	}
	if (count > 1)
		count--;
	return count;
}

/***** state handlers *****/
state_result init_state_handler(va_list* args, state_args* state_args) {
	enum printf_state state;
	int out_printed_chars = 0;
	switch (*(state_args->fs)) {
	case '\\':
		state = st_printf_meta_char;
		break;
	case '%':
		state = st_printf_percent;
		break;

	default:
		putchar(*(state_args->fs));
		++out_printed_chars;
		state = st_printf_init;
	}
	state_result res; 
	res.next_state = state; 
	res.printed_chars = out_printed_chars;
	return res;
}

state_result meta_char_state_handler(va_list* args, state_args* state_args) {
	enum printf_state state;
	int out_printed_chars = 0;
	switch (*state_args->fs) {
	case '\\':
		putchar('\\');
		++out_printed_chars;
		state = st_printf_init;
		break;

	case '\"':
		putchar('\"');
		++out_printed_chars;
		state = st_printf_init;
		break;

	case 't':
		putchar('\t');
		++out_printed_chars;
		state = st_printf_init;
		break;

	case 'T':
		out_printed_chars += toy_printf("<tab>\t");
		state = st_printf_init;
		break;

	case 'f':
		putchar('\f');
		++out_printed_chars;
		state = st_printf_init;
		break;

	case 'F':
		out_printed_chars += toy_printf("<formfeed>\f");
		state = st_printf_init;
		break;

	case 'n':
		putchar('\n');
		++out_printed_chars;
		state = st_printf_init;
		break;

	case 'N':
		out_printed_chars += toy_printf("<newline>\n");
		state = st_printf_init;
		break;

	case 'r':
		putchar('\r');
		++out_printed_chars;
		state = st_printf_init;
		break;

	case 'R':
		out_printed_chars += toy_printf("<return>\r");
		state = st_printf_init;
		break;

	case 'c':
		out_printed_chars += toy_printf("CASPL'2018");
		state = st_printf_init;
		break;

	case 'C':
		out_printed_chars += toy_printf("<caspl magic>");
		out_printed_chars += toy_printf("\\c");
		state = st_printf_init;
		break;

	default:
		if (is_octal_char(*state_args->fs)) {
			state_args->octal_char = *state_args->fs - '0';
			state = st_printf_octal2;
		}
		else {
			toy_printf("Unknown meta-character: \\%c", *(state_args->fs));
			exit(-1);
		}
	}
	state_result res;
	res.next_state = state;
	res.printed_chars = out_printed_chars;
	return res;
}

state_result percent_state_handler(va_list* args, state_args* state_args) {
	enum printf_state state;
	int out_printed_chars = 0;
	int int_value;
	char* string_value;
	char char_value;
	int array_size;
	int int_to_pad;
	char* string_to_pad;
	void* array_of_voids;
	int padding;
	char* fs = state_args->fs;
	switch (*fs) {
	case '%':
		putchar('%');
		++out_printed_chars;
		state = st_printf_init;
		break;

	case 'A':
		array_of_voids = va_arg(args, void*);
		array_size = va_arg(args, int);
		out_printed_chars += print_array_helper(array_of_voids, array_size, fs);
		fs++;
		state = st_printf_init;
		break;

	case 'd':
		int_value = va_arg(args, int);
		out_printed_chars += print_int(int_value, 10, digit);
		state = st_printf_init;
		break;

	case 'u':
		int_value = va_arg(args, int);
		if (int_value < 0) {
			out_printed_chars += print_int_helper(int_value, 10, digit);
		}
		else {
			out_printed_chars += print_int(int_value, 10, digit);
		}
		state = st_printf_init;
		break;

	case 'b':
		int_value = va_arg(args, int);
		out_printed_chars += print_int(int_value, 2, digit);
		state = st_printf_init;
		break;

	case 'o':
		int_value = va_arg(args, int);
		out_printed_chars += print_int(int_value, 8, digit);
		state = st_printf_init;
		break;

	case 'x':
		int_value = va_arg(args, int);
		out_printed_chars += print_int(int_value, 16, digit);
		state = st_printf_init;
		break;

	case 'X':
		int_value = va_arg(args, int);
		out_printed_chars += print_int(int_value, 16, DIGIT);
		state = st_printf_init;
		break;

	case 's':
		string_value = va_arg(args, char *);
		out_printed_chars += toy_printf(string_value);
		state = st_printf_init;
		break;

	case 'c':
		char_value = (char)va_arg(args, int);
		putchar(char_value);
		++out_printed_chars;
		state = st_printf_init;
		break;

	case '0': // DEAL WITH INT ZERO PADDING
		fs++;
		padding = atoi(fs);
		while (*(++fs) >= '0' && *fs <= '9') {}
		if (*fs == 'd') {
			int_to_pad = va_arg(args, int);
			if (int_to_pad < 0) {
				putchar('-');
				out_printed_chars++;
				int_to_pad = int_to_pad * (-1);
				padding--;
			}
			padding = padding - count_int_digits(int_to_pad);
			for (int i = 0; i < padding; ++i) {
				putchar('0');
				out_printed_chars++;
			}
			out_printed_chars += toy_printf("%d", int_to_pad);
			state = st_printf_init;
		}
		else {
			exit(-1);
		}
		break;

	case '-': // DEAL WITH LEFT PADDING
		(fs++);
		padding = atoi(fs);
		while (*(++fs) >= '0' && *fs <= '9') {}
		switch (*fs) {
		case 'd':
			int_to_pad = va_arg(args, int);
			padding = padding - count_int_digits(int_to_pad);
			for (int i = 0; i < padding; ++i) {
				putchar(' ');
				out_printed_chars++;
			}
			out_printed_chars += toy_printf("%d", int_to_pad);
			break;
		case 's':
			string_to_pad = va_arg(args, char*);
			padding = padding - strlen(string_to_pad);
			for (int i = 0; i < padding; ++i) {
				putchar(' ');
				out_printed_chars++;
			}
			out_printed_chars += toy_printf("%s", string_to_pad);
			break;
		default:
			exit(-1);
		}
		state = st_printf_init;
		break;

	default:
		if (*fs > '0' && *fs <= '9') { 	// DEAL WITH RIGHT PADDING
			padding = atoi(fs);
			while (*(++fs) >= '0' && *fs <= '9') {}
			switch (*fs) {
			case 'd':
				int_to_pad = va_arg(args, int);
				padding = padding - count_int_digits(int_to_pad);
				out_printed_chars += toy_printf("%d", int_to_pad);
				break;
			case 'u':
				int_to_pad = va_arg(args, int);
				padding = padding - count_int_digits(int_to_pad);
				out_printed_chars += toy_printf("%u", int_to_pad);
				break;
			case 'b':
				int_to_pad = va_arg(args, int);
				padding = padding - count_int_digits(int_to_pad);
				out_printed_chars += toy_printf("%b", int_to_pad);
				break;
			case 'x':
				int_to_pad = va_arg(args, int);
				padding = padding - count_int_digits(int_to_pad);
				out_printed_chars += toy_printf("%x", int_to_pad);
				break;
			case 'X':
				int_to_pad = va_arg(args, int);
				padding = padding - count_int_digits(int_to_pad);
				out_printed_chars += toy_printf("%X", int_to_pad);
				break;
			case 's':
				string_to_pad = va_arg(args, char*);
				padding = padding - strlen(string_to_pad);
				out_printed_chars += toy_printf("%s", string_to_pad);
				break;
			default:
				exit(-1);
			}
			if (padding > 0) {
				for (int i = 0; i < padding - 1; ++i) {
					putchar(' ');
					out_printed_chars++;
				}
				putchar('#');
				out_printed_chars++;
			}
			state = st_printf_init;
		}
		else {
			toy_printf("Unhandled format %%%c...\n", *fs);
			exit(-1);
		}
	}
	(*state_args).fs = fs;
	state_result res;
	res.next_state = state;
	res.printed_chars = out_printed_chars;
	return res;
}

state_result octal2_state_handler(va_list* arg, state_args* state_args) {
	enum printf_state state;
	int out_printed_chars = 0;
	if (is_octal_char(*(state_args->fs))) {
		state_args->octal_char = (state_args->octal_char << 3) + (*(state_args->fs) - '0');
		state = st_printf_octal3;
	}
	else {
		toy_printf("Missing second octal char. Found: \\%c", *(state_args->fs));
		exit(-1);
	}
	state_result res;
	res.next_state = state;
	res.printed_chars = out_printed_chars;
	return res;
}
state_result octal3_state_handler(va_list* args, state_args* state_args) {
	enum printf_state state;
	int out_printed_chars = 0;
	if (is_octal_char(*(state_args->fs))) {
		state_args->octal_char = (state_args->octal_char << 3) + (*(state_args->fs) - '0');
		putchar(state_args->octal_char);
		++out_printed_chars;
		state = st_printf_init;
		}
	else {
		toy_printf("Missing third octal char. Found: \\%c", *(state_args->fs));
		exit(-1);
	}
	state_result res;
	res.next_state = state;
	res.printed_chars = out_printed_chars;
	return res;

}

state_result(*state_handlers_arr[NUM_OF_STATES]) (va_list* args, state_args* state_args);

void initial_state_handlers_arr() {
	state_handlers_arr[st_printf_init] = init_state_handler;
	state_handlers_arr[st_printf_meta_char] = meta_char_state_handler;
	state_handlers_arr[st_printf_octal2] = octal2_state_handler;
	state_handlers_arr[st_printf_octal3] = octal3_state_handler;
	state_handlers_arr[st_printf_percent] = percent_state_handler;
}

/* SUPPORTED:
 *   \f, \t, \n, \r -- formfeed, tab, newline, return
 *   \F, \T, \N, \R -- extensions: visible versions of the above
 *   \c -- extension: CASPL'2018
 *   \C -- extension: visible version of the above
 *   %b, %d, %o, %x, %X, %u --
 *     signed integers in binary, decimal, unsigned integers in octal, hex, HEX, and decimal
 *   %s -- strings
 *	 %Ad, %As, ... -- array of integers, strings, ...
 */

int toy_printf(char *fs, ...) {
	int chars_printed = 0;
	va_list args;
	state_args sa;
	state_result sr;
	enum printf_state state;

	va_start(args, fs);
	initial_state_handlers_arr();

	state = st_printf_init;

	for (; *fs != '\0'; ++fs) {
		sa.fs = fs;
		sr = (state_handlers_arr[state])(&args, &sa);
		state = sr.next_state;
		chars_printed += sr.printed_chars;
		fs = sa.fs;
	}

	va_end(args);

	return chars_printed;
}