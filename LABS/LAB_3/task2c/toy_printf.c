#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* the states in the printf state-machine */

enum printf_state {
	st_printf_init,
	st_printf_percent
};

#define ASCII_SIZE 128
#define NUM_OF_STATES 2
#define TRUE 1
#define FALSE 0
#define MAX_NUMBER_LENGTH 64

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int toy_printf(char *fs, ...);

typedef struct state_args {
	char* fs;
} state_args;

typedef struct state_result {
	int printed_chars;
	enum printf_state next_state;
} state_result;

int count_int_digits(int int_to_pad) {
	int count = 1;
	while (int_to_pad != 0) {
		int_to_pad /= 10;
		++count;
	}
	if (count > 1)
		count--;
	return count;
}

/* print helpers */

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

/*****  PRINT ARRAY HELPER handlers and handler function pointers  *****/

int print_array_helper_handler_for_numbers(void* arr, int array_size, char* fs) {
	int *int_arr;
	int num_of_chars = 0;
	char print_instruction[3] = { '%', *fs, '\0' };
	int_arr = (int*)arr;
	num_of_chars += toy_printf(print_instruction, int_arr[0]);
	for (int i = 1; i < array_size; ++i) {
		num_of_chars += toy_printf(", ");
		num_of_chars += toy_printf(print_instruction, int_arr[i]);
	}
	return num_of_chars;
}
int print_array_helper_handler_for_strings(void* arr, int array_size, char* fs) {
	char **string_arr;
	int num_of_chars = 0;
	char print_instruction[3] = { '%', *fs, '\0' };
	string_arr = (char**)arr;
	num_of_chars += toy_printf("\"");
	num_of_chars += toy_printf(print_instruction, string_arr[0]);
	num_of_chars += toy_printf("\"");
	for (int i = 1; i < array_size; ++i) {
		num_of_chars += toy_printf(", ");
		num_of_chars += toy_printf("\"");
		num_of_chars += toy_printf(print_instruction, string_arr[i]);
		num_of_chars += toy_printf("\"");
	}
	return num_of_chars;
}
int print_array_helper_handler_for_chars(void* arr, int array_size, char* fs) {
	char *char_arr;
	int num_of_chars = 0;
	char print_instruction[3] = { '%', *fs, '\0' };
	char_arr = (char*)arr;
	num_of_chars += toy_printf("\'");
	num_of_chars += toy_printf(print_instruction, char_arr[0]);
	num_of_chars += toy_printf("\'");
	for (int i = 1; i < array_size; ++i) {
		num_of_chars += toy_printf(", ");
		num_of_chars += toy_printf("\'");
		num_of_chars += toy_printf(print_instruction, char_arr[i]);
		num_of_chars += toy_printf("\'");
	}
	return num_of_chars;
}


int (*print_array_helper_handlers_arr[ASCII_SIZE]) (void* arr, int array_size, char* fs);

void init_print_array_helper_handlers_arr() {
	print_array_helper_handlers_arr['s'] = print_array_helper_handler_for_strings;
	print_array_helper_handlers_arr['c'] = print_array_helper_handler_for_chars;
        print_array_helper_handlers_arr['u'] = print_array_helper_handler_for_numbers;
        print_array_helper_handlers_arr['d'] = print_array_helper_handler_for_numbers;
        print_array_helper_handlers_arr['x'] = print_array_helper_handler_for_numbers;
        print_array_helper_handlers_arr['X'] = print_array_helper_handler_for_numbers;
        print_array_helper_handlers_arr['b'] = print_array_helper_handler_for_numbers;
        print_array_helper_handlers_arr['o'] = print_array_helper_handler_for_numbers;
}

int print_array_helper(void* arr, int array_size, char* fs){
	int num_of_chars = 0;
	putchar('{');
	int handler = *(++fs);
	num_of_chars += (print_array_helper_handlers_arr[handler])(arr, array_size, fs);
	putchar('}');
	num_of_chars += 2; // {}
	return num_of_chars;
}


/*  state handlers  */


/*****  INIT STATE handler functions and function pointers  *****/

enum printf_state init_state_handler_for_percent(va_list args, state_args* state_args, int* out_printed_chars) {
	return st_printf_percent;
}
enum printf_state init_state_handler_for_regular_chars(va_list args, state_args* state_args, int* out_printed_chars) {
	putchar(*(state_args->fs));
	++(*out_printed_chars);
	return st_printf_init;
}

enum printf_state (*init_state_handler_arr[ASCII_SIZE]) (va_list, state_args*, int*);

int not_init_special(int i) { 
	return (i != '%') ? TRUE : FALSE; 
}

void init_init_state_handler_arr() {
	init_state_handler_arr['%'] = init_state_handler_for_percent;
	for (int i = 0; i < ASCII_SIZE; i++) {
		if (not_init_special(i)) {
			init_state_handler_arr[i] = init_state_handler_for_regular_chars;
		}
	}
}

state_result init_state_handler(va_list args, state_args* state_args) {
	enum printf_state state;
	int out_printed_chars = 0;
	int handler = *(state_args->fs);
	state = (init_state_handler_arr[handler])(args, state_args, &out_printed_chars);
	state_result res; 
	res.next_state = state; 
	res.printed_chars = out_printed_chars;
	return res;
}


/*****  PERCENT STATE handler functions and function pointers  *****/

enum printf_state percent_state_handler_for_percent(va_list args, state_args* state_args, int* out_printed_chars) {
	putchar('%');
	++(*out_printed_chars);
	return st_printf_init;
}
enum printf_state percent_state_handler_for_arrays(va_list args, state_args* state_args, int* out_printed_chars) {
	int array_size;
	void* array_of_voids;
	array_of_voids = va_arg(args, void*);
	array_size = va_arg(args, int);
	*out_printed_chars += print_array_helper(array_of_voids, array_size, state_args->fs);
	(state_args->fs)++;
	return st_printf_init;
}
enum printf_state percent_state_handler_for_d(va_list args, state_args* state_args, int* out_printed_chars) {
	int int_value;
	int_value = va_arg(args, int);
	*out_printed_chars += print_int(int_value, 10, digit);
	return st_printf_init;
}
enum printf_state percent_state_handler_for_u(va_list args, state_args* state_args, int* out_printed_chars) {
	int int_value;
	int_value = va_arg(args, int);
	if (int_value < 0) {
		*out_printed_chars += print_int_helper(int_value, 10, digit);
	}
	else {
		*out_printed_chars += print_int(int_value, 10, digit);
	}
	return st_printf_init;
}
enum printf_state percent_state_handler_for_b(va_list args, state_args* state_args, int* out_printed_chars) {
	int int_value;
	int_value = va_arg(args, int);
	*out_printed_chars += print_int(int_value, 2, digit);
	return st_printf_init;
}
enum printf_state percent_state_handler_for_o(va_list args, state_args* state_args, int* out_printed_chars) {
	int int_value;
	int_value = va_arg(args, int);
	*out_printed_chars += print_int(int_value, 8, digit);
	return st_printf_init;
}
enum printf_state percent_state_handler_for_x(va_list args, state_args* state_args, int* out_printed_chars) {
	int int_value;
	int_value = va_arg(args, int);
	*out_printed_chars += print_int(int_value, 16, digit);
	return st_printf_init;
}
enum printf_state percent_state_handler_for_X(va_list args, state_args* state_args, int* out_printed_chars) {
	int int_value;
	int_value = va_arg(args, int);
	*out_printed_chars += print_int(int_value, 16, DIGIT);
	return st_printf_init;
}
enum printf_state percent_state_handler_for_s(va_list args, state_args* state_args, int* out_printed_chars) {
	char* string_value;
	string_value = va_arg(args, char *);
	*out_printed_chars += toy_printf(string_value);
	return st_printf_init;
}
enum printf_state percent_state_handler_for_c(va_list args, state_args* state_args, int* out_printed_chars) {
	char char_value;
	char_value = (char)va_arg(args, int);
	putchar(char_value);
	++(*out_printed_chars);
	return st_printf_init;
}
enum printf_state percent_state_handler_for_zero_padding(va_list args, state_args* state_args, int* out_printed_chars) { // '0'
	int padding;
	int int_to_pad;
	state_args->fs++;
	padding = atoi(state_args->fs);
	while (*(++state_args->fs) >= '0' && *state_args->fs <= '9') {}
	if (*state_args->fs == 'd') {
		int_to_pad = va_arg(args, int);
		if (int_to_pad < 0) {
			putchar('-');
			++(*out_printed_chars);
			int_to_pad = int_to_pad * (-1);
			padding--;
		}
		padding = padding - count_int_digits(int_to_pad);
		for (int i = 0; i < padding; ++i) {
			putchar('0');
			++(*out_printed_chars);
		}
		*out_printed_chars += toy_printf("%d", int_to_pad);
		return st_printf_init;
	}
	else {
		exit(-1);
	}
}


/* LEFT PADDING function handlers and function pointers */

void left_padding_for_numbers(va_list args, state_args* state_args, int* out_printed_chars, int padding){
    int int_to_pad;
    int_to_pad = va_arg(args, int);
    padding = padding - count_int_digits(int_to_pad);
    for (int i = 0; i < padding; ++i) {
            putchar(' ');
            ++(*out_printed_chars);
    }
    *out_printed_chars += toy_printf("%d", int_to_pad);
}
void left_padding_for_strings(va_list args, state_args* state_args, int* out_printed_chars, int padding){
    char* string_to_pad;
    string_to_pad = va_arg(args, char*);
    padding = padding - strlen(string_to_pad);
    for (int i = 0; i < padding; ++i) {
            putchar(' ');
            ++(*out_printed_chars);
    }
    *out_printed_chars += toy_printf("%s", string_to_pad);
}

void (*left_padding_handlers_arr[ASCII_SIZE])(va_list args, state_args* state_args, int* out_printed_chars, int padding);

void init_left_padding_handlers_arr(){
    left_padding_handlers_arr['d'] = left_padding_for_numbers;
    left_padding_handlers_arr['s'] = left_padding_for_strings;
}

enum printf_state percent_state_handler_for_left_padding(va_list args, state_args* state_args, int* out_printed_chars) { // '-'
	int padding;
        (state_args->fs++);
	padding = atoi(state_args->fs);
	while (*(++state_args->fs) >= '0' && *state_args->fs <= '9') {}
	int handler = *state_args->fs;
	(left_padding_handlers_arr[handler])(args, state_args, out_printed_chars, padding);
	return st_printf_init;
}

/* RIGHT PADDING function handlers and function pointers */

int right_padding_for_numbers(va_list args, state_args* state_args, int* out_printed_chars, int padding){
    int int_to_pad;
    int_to_pad = va_arg(args, int);
    padding = padding - count_int_digits(int_to_pad);
    *out_printed_chars += toy_printf("%d", int_to_pad);
    return padding;
}
int right_padding_for_strings(va_list args, state_args* state_args, int* out_printed_chars, int padding){
    char* string_to_pad;
    string_to_pad = va_arg(args, char*);
    padding = padding - strlen(string_to_pad);
    *out_printed_chars += toy_printf("%s", string_to_pad);
    return padding;
}

int (*right_padding_handlers_arr[ASCII_SIZE])(va_list args, state_args* state_args, int* out_printed_chars, int padding);

void init_right_padding_handlers_arr(){
    right_padding_handlers_arr['d'] = right_padding_for_numbers;
    right_padding_handlers_arr['s'] = right_padding_for_strings;
}

enum printf_state percent_state_handler_for_right_padding(va_list args, state_args* state_args, int* out_printed_chars) { // '1' -> '9'
	int padding;
	padding = atoi(state_args->fs);
	while (*(++state_args->fs) >= '0' && *state_args->fs <= '9') {}
        int handler = *state_args->fs;
        padding = (right_padding_handlers_arr[handler])(args, state_args, out_printed_chars, padding);
	if (padding > 0) {
		for (int i = 0; i < padding - 1; ++i) {
			putchar(' ');
			++(*out_printed_chars);
		}
		putchar('#');
		++(*out_printed_chars);
	}
	return st_printf_init;
}

enum printf_state(*percent_state_handler_arr[ASCII_SIZE]) (va_list, state_args*, int*);

int right_padding_char(char c) {
	return (c >= '1' && c <= '9') ? TRUE : FALSE;
}

void init_percent_state_handler_arr() {
	percent_state_handler_arr['%'] = percent_state_handler_for_percent;
	percent_state_handler_arr['A'] = percent_state_handler_for_arrays;
	percent_state_handler_arr['d'] = percent_state_handler_for_d;
	percent_state_handler_arr['u'] = percent_state_handler_for_u;
	percent_state_handler_arr['b'] = percent_state_handler_for_b;
	percent_state_handler_arr['o'] = percent_state_handler_for_o;
	percent_state_handler_arr['x'] = percent_state_handler_for_x;
	percent_state_handler_arr['X'] = percent_state_handler_for_X;
	percent_state_handler_arr['s'] = percent_state_handler_for_s;
	percent_state_handler_arr['c'] = percent_state_handler_for_c;
	percent_state_handler_arr['0'] = percent_state_handler_for_zero_padding;
	percent_state_handler_arr['-'] = percent_state_handler_for_left_padding;
	for (int i = 0; i < ASCII_SIZE; i++) {
		if (right_padding_char(i)) {
			percent_state_handler_arr[i] = percent_state_handler_for_right_padding;
		}
	}
}

state_result percent_state_handler(va_list args, state_args* state_args) {
	enum printf_state state;
	int out_printed_chars = 0;
	int handler = *state_args->fs;
	state = (percent_state_handler_arr[handler])(args, state_args, &out_printed_chars);
	state_result res;
	res.next_state = state;
	res.printed_chars = out_printed_chars;
	return res;
}


/*****  STATE HANDLER function pointers  *****/

state_result(*state_handlers_arr[NUM_OF_STATES]) (va_list args, state_args* state_args);

void init_state_handlers_arr() {
	state_handlers_arr[st_printf_init] = init_state_handler;
	state_handlers_arr[st_printf_percent] = percent_state_handler;
}


/*  toy_printf start  */

int toy_printf(char *fs, ...) {
	int chars_printed = 0;
	va_list args;
	state_args sa;
	state_result sr;
	enum printf_state state;

	va_start(args, fs);

	// init handlers arrays
	init_state_handlers_arr();
	init_print_array_helper_handlers_arr();
	init_init_state_handler_arr();
	init_percent_state_handler_arr();
        init_left_padding_handlers_arr();
        init_right_padding_handlers_arr();

	state = st_printf_init;

	for (; *fs != '\0'; ++fs) {
		sa.fs = fs;
		sr = (state_handlers_arr[state])(args, &sa);
		state = sr.next_state;
		chars_printed += sr.printed_chars;
		fs = sa.fs;
	}

	va_end(args);

	return chars_printed;
}