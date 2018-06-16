
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

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

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
		if (radix != 10) { /*********#########********/
			return print_int_helper(n, radix, digit);
		}
		else {
			putchar('-');
			return 1 + print_int_helper(-n, radix, digit);
		}
	}
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
	int int_value = 0;
	char *string_value;
	char char_value;
	char octal_char;
	int array_size;
	int *int_arr;
	unsigned int *uint_arr;
	int *bint_arr;
	int *oint_arr;
	int *xint_arr;
	int *Xint_arr;
	char **string_arr;
	char *char_arr;
	int int_to_pad;
	char *string_to_pad;

	va_list args;
	enum printf_state state;

	va_start(args, fs);

	state = st_printf_init;

	for (; *fs != '\0'; ++fs) {
		switch (state) {
		case st_printf_init:
			switch (*fs) {
			case '\\':
				state = st_printf_meta_char;
				break;

			case '%':
				state = st_printf_percent;
				break;

			default:
				putchar(*fs);
				++chars_printed;
			}
			break;

		case st_printf_meta_char:
			switch (*fs) {
			case '\\':
				putchar('\\');
				++chars_printed;
				state = st_printf_init;
				break;

			case '\"':
				putchar('\"');
				++chars_printed;
				state = st_printf_init;
				break;

			case 't':
				putchar('\t');
				++chars_printed;
				state = st_printf_init;
				break;

			case 'T':
				chars_printed += toy_printf("<tab>\t");
				state = st_printf_init;
				break;

			case 'f':
				putchar('\f');
				++chars_printed;
				state = st_printf_init;
				break;

			case 'F':
				chars_printed += toy_printf("<formfeed>\f");
				state = st_printf_init;
				break;

			case 'n':
				putchar('\n');
				++chars_printed;
				state = st_printf_init;
				break;

			case 'N':
				chars_printed += toy_printf("<newline>\n");
				state = st_printf_init;
				break;

			case 'r':
				putchar('\r');
				++chars_printed;
				state = st_printf_init;
				break;

			case 'R':
				chars_printed += toy_printf("<return>\r");
				state = st_printf_init;
				break;

			case 'c':
				chars_printed += toy_printf("CASPL'2018");
				state = st_printf_init;
				break;

			case 'C':
				chars_printed += toy_printf("<caspl magic>");
				chars_printed += toy_printf("\\c");
				state = st_printf_init;
				break;

			default:
				if (is_octal_char(*fs)) {
					octal_char = *fs - '0';
					state = st_printf_octal2;
				}
				else {
					toy_printf("Unknown meta-character: \\%c", *fs);
					exit(-1);
				}
			}
			break;

		case st_printf_octal2:
			if (is_octal_char(*fs)) {
				octal_char = (octal_char << 3) + (*fs - '0');
				state = st_printf_octal3;
				break;
			}
			else {
				toy_printf("Missing second octal char. Found: \\%c", *fs);
				exit(-1);
			}

		case st_printf_octal3:
			if (is_octal_char(*fs)) {
				octal_char = (octal_char << 3) + (*fs - '0');
				putchar(octal_char);
				++chars_printed;
				state = st_printf_init;
				break;
			}
			else {
				toy_printf("Missing third octal char. Found: \\%c", *fs);
				exit(-1);
			}

		case st_printf_percent:
			switch (*fs) {
			case '%':
				putchar('%');
				++chars_printed;
				state = st_printf_init;
				break;

			case 'A':
				switch (*(++fs)) {
				case 'd':
					int_arr = va_arg(args, int*);
					array_size = va_arg(args, int);
					putchar('{');
					chars_printed += toy_printf("%d", int_arr[0]);
					for (int i = 1; i < array_size; ++i) {
						chars_printed += toy_printf(", %d", int_arr[i]);
					}
					putchar('}');
					chars_printed += 2;
					break;
				case 'u':
					uint_arr = va_arg(args, unsigned int*);
					array_size = va_arg(args, int);
					putchar('{');
					chars_printed += toy_printf("%u", uint_arr[0]);
					for (int i = 1; i < array_size; ++i) {
						chars_printed += toy_printf(", %u", uint_arr[i]);
					}
					putchar('}');
					chars_printed += 2;
					break;
				case 'b':
					bint_arr = va_arg(args, int*);
					array_size = va_arg(args, int);
					putchar('{');
					chars_printed += toy_printf("%b", bint_arr[0]);
					for (int i = 1; i < array_size; ++i) {
						chars_printed += toy_printf(", %b", bint_arr[i]);
					}
					putchar('}');
					chars_printed += 2;
					break;
				case 'o':
					oint_arr = va_arg(args, int*);
					array_size = va_arg(args, int);
					putchar('{');
					chars_printed += toy_printf("%o", oint_arr[0]);
					for (int i = 1; i < array_size; ++i) {
						chars_printed += toy_printf(", %o", oint_arr[i]);
					}
					putchar('}');
					chars_printed += 2;
					break;
				case 'x':
					xint_arr = va_arg(args, int*);
					array_size = va_arg(args, int);
					putchar('{');
					chars_printed += toy_printf("%x", xint_arr[0]);
					for (int i = 1; i < array_size; ++i) {
						chars_printed += toy_printf(", %x", xint_arr[i]);
					}
					putchar('}');
					chars_printed += 2;
					break;
				case 'X':
					Xint_arr = va_arg(args, int*);
					array_size = va_arg(args, int);
					putchar('{');
					chars_printed += toy_printf("%X", Xint_arr[0]);
					for (int i = 1; i < array_size; ++i) {
						chars_printed += toy_printf(", %X", Xint_arr[i]);
					}
					putchar('}');
					chars_printed += 2;
					break;
				case 's':
					string_arr = va_arg(args, char**);
					array_size = va_arg(args, int);
					putchar('{');
					chars_printed += toy_printf("\"%s\"", string_arr[0]);
					for (int i = 1; i < array_size; ++i) {
						chars_printed += toy_printf(", \"%s\"", string_arr[i]);
					}
					putchar('}');
					chars_printed += 2;
					break;
				case 'c':
					char_arr = va_arg(args, char*);
					array_size = va_arg(args, int);
					putchar('{');
					chars_printed += toy_printf("\"%c\"", char_arr[0]);
					for (int i = 1; i < array_size; ++i) {
						chars_printed += toy_printf(", \"%c\"", char_arr[i]);
					}
					putchar('}');
					chars_printed += 2;
					break;
				}
				state = st_printf_init;
				break;

			case 'd':
				int_value = va_arg(args, int);
				chars_printed += print_int(int_value, 10, digit);
				state = st_printf_init;
				break;

			case 'u':
				int_value = va_arg(args, int);
				if (int_value < 0) {
					chars_printed += print_int_helper(int_value, 10, digit);
				}
				else {
					chars_printed += print_int(int_value, 10, digit);
				}
				state = st_printf_init;
				break;

			case 'b':
				int_value = va_arg(args, int);
				chars_printed += print_int(int_value, 2, digit);
				state = st_printf_init;
				break;

			case 'o':
				int_value = va_arg(args, int);
				chars_printed += print_int(int_value, 8, digit);
				state = st_printf_init;
				break;

			case 'x':
				int_value = va_arg(args, int);
				chars_printed += print_int(int_value, 16, digit);
				state = st_printf_init;
				break;

			case 'X':
				int_value = va_arg(args, int);
				chars_printed += print_int(int_value, 16, DIGIT);
				state = st_printf_init;
				break;

			case 's':
				string_value = va_arg(args, char *);
				chars_printed += toy_printf(string_value);
				state = st_printf_init;
				break;

			case 'c':
				char_value = (char)va_arg(args, int);
				putchar(char_value);
				++chars_printed;
				state = st_printf_init;
				break;

			default:
				if (*fs > '0' && *fs <= '9') {
					// DEAL WITH RIGHT PADDING
					int padding = atoi(fs);
					while (*(++fs) >= '0' && *fs <= '9') {}
					switch (*fs) {
					case 'd':
						int_to_pad = va_arg(args, int);
						padding = padding - count_int_digits(int_to_pad);
						chars_printed += toy_printf("%d", int_to_pad);
						break;
					case 's':
						string_to_pad = va_arg(args, char*);
						padding = padding - strlen(string_to_pad);
						chars_printed += toy_printf("%s", string_to_pad);
						break;
					default:
						exit(-1);
					}
					if (padding > 0) {
						for (int i = 0; i < padding-1; ++i) {
							putchar(' ');
							chars_printed++;
						}
						putchar('#');
						chars_printed++;
					}
					state = st_printf_init;
				}
				else if (*fs == '0') {
					// DEAL WITH INT ZERO PADDING
					fs++;
					int padding = atoi(fs);
					while (*(++fs) >= '0' && *fs <= '9') {}
					if (*fs == 'd') {
						int_to_pad = va_arg(args, int);
						if (int_to_pad < 0) {
							putchar('-');
							chars_printed++;
							int_to_pad = int_to_pad * (-1);
							padding--;
						}
						padding = padding - count_int_digits(int_to_pad);
						for (int i = 0; i < padding; ++i) {
							putchar('0');
							chars_printed++;
						}
						chars_printed += toy_printf("%d", int_to_pad);
						state = st_printf_init;
					}
					else
						exit(-1);
				}
				else if (*fs == '-') {
					// DEAL WITH LEFT PADDING
					(fs++);
					int padding = atoi(fs);
					while (*(++fs) >= '0' && *fs <= '9') {}
					switch (*fs) {
					case 'd':
						int_to_pad = va_arg(args, int);
						padding = padding - count_int_digits(int_to_pad);
						for (int i = 0; i < padding; ++i) {
							putchar(' ');
							chars_printed++;
						}
						chars_printed += toy_printf("%d", int_to_pad);
						break;
					case 's':
						string_to_pad = va_arg(args, char*);
						padding = padding - strlen(string_to_pad);
						for (int i = 0; i < padding; ++i) {
							putchar(' ');
							chars_printed++;
						}
						chars_printed += toy_printf("%s", string_to_pad);
						break;
					default:
						exit(-1);
					}
					state = st_printf_init;
				}
				else {
					toy_printf("Unhandled format %%%c...\n", *fs);
					exit(-1);
				}
			}
			break;

		default:
			toy_printf("toy_printf: Unknown state -- %d\n", (int)state);
			exit(-1);
		}
	}

	va_end(args);

	return chars_printed;
}