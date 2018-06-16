#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_SIZE 1024
#define TRUE 1  
#define FALSE 0
#define BUF_SIZE 1024
#define INIT_SIZE 32
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct bignum{
    long number_of_digits;
    char* digits;
} bignum;

//
typedef struct ST{
    bignum* arr[MAX_STACK_SIZE];
    int head;
} ST;

ST stack;

extern char* bnadd(bignum*, bignum*);
extern char* bnsub(bignum*, bignum*);
extern char* div_by_2(bignum*);
extern int mod_2(bignum*);

char * dispatch_ops(bignum*, bignum*, char);
int negative(bignum * op) { return op->digits[0] == '_' ? 1 : 0; }
int compare_bignums(bignum * op1, bignum * op2);
long lngstrlen(const char *s);
void lngstrncpy(char *dest, const char *src, long n);
int one(bignum* bn);
void divide(bignum ** first, bignum ** second, bignum ** result, bignum ** factor);
void div_helper(bignum ** first, bignum ** second, bignum ** result, bignum ** factor);
void printbn(bignum *bn);

void init_stack(){
    stack.head = -1;
}

bignum* pop(){
    if(stack.head == -1)
        return NULL;
    return stack.arr[stack.head--];
}

int is_empty(){
    return stack.head == -1 ? TRUE : FALSE;
}

int push(bignum* bn){
    if(stack.head == MAX_STACK_SIZE - 1)
        return -1;
    stack.arr[++stack.head] = bn;
    return 0;
}

bignum* peek(){
    if (stack.head == -1)
        return NULL;
    return stack.arr[stack.head];
}

bignum* init_big_num(){
    bignum* bnp = (bignum*)malloc(sizeof(bignum));
    bnp->number_of_digits = 0;
    bnp->digits = (char*)malloc(INIT_SIZE * sizeof(char));
    return bnp;
}

void print_stack(){
    printf("printing the stack from top to bottom\n");
    for(int i = 0 ; i <= stack.head ; i++){
        printf("bignum at index %d:\n\tnum_of_digits: %ld\n\tnumber: %s\n", i,
                                                                            stack.arr[i]->number_of_digits,
                                                                            stack.arr[i]->digits
                                                                            );
    }
}

int is_digit(char c){ 
    return ( c >= '0' && c <= '9' ) ? TRUE : FALSE;
}
int is_operation(char c){
    return ( c == '+' || c == '-' || c == '*' || c == '/' ) ? TRUE : FALSE;
}

void free_stack(){
    bignum * op;
    while((op = pop()) != NULL){
        free(op->digits);
        free(op);          
    }
}

int zero(char * digits){
    digits++;
    for(; *digits ; digits++){
        if(*digits != '0')
            return FALSE;
    }
    return TRUE;
}

bignum * fill_big_num(char * digits){
    bignum* bnp = (bignum*)malloc(sizeof(bignum));
    long n = lngstrlen(digits);
    char * digits_idx = digits + 1;
    int num_of_zeroes = 0;
    while(*digits_idx == '0'){
        num_of_zeroes++;
        digits_idx++;
    }
    if(num_of_zeroes == n - 1){ //number is 0
        bnp->number_of_digits = 1;
        bnp->digits = (char*)malloc(3); //sign, '0', '\0'
        bnp->digits[0] = '+';
        bnp->digits[1] = '0';
        bnp->digits[2] = '\0';
        return bnp;
    }
    *(--digits_idx) = *digits;
    long new_num_of_digits = n - num_of_zeroes;
    char* new_digits = (char*)malloc(new_num_of_digits + 1);
    lngstrncpy(new_digits, digits_idx, new_num_of_digits);
    free(digits);
    bnp->digits = new_digits;
    bnp->number_of_digits = new_num_of_digits - 1; // sign
    bnp->digits[bnp->number_of_digits + 1] = '\0';
    return bnp;
}

int main(int argc, char* argv[]) {
    init_stack();
    char c;
    while((c = getc(stdin)) != 'q'){
        if(is_digit(c) || c == '_'){
            bignum * bnp = init_big_num(); // allocate bignum holder
            if(is_digit(c)) bnp->digits[bnp->number_of_digits++] = '+';
            bnp->digits[bnp->number_of_digits++] = c;
            while((c = getc(stdin)) && c >= '0' && c <= '9'){
                if(bnp->number_of_digits % (INIT_SIZE - 2) == 0) // '+' /'_', '\0'
                    bnp->digits = (char*)realloc(bnp->digits, INIT_SIZE * sizeof(char) + bnp->number_of_digits);           	
                bnp->digits[bnp->number_of_digits++] = c; // put this char in his place
            }
            bnp->digits[bnp->number_of_digits] = '\0'; // null-terminate the digits array
            bnp->number_of_digits--;
            push(bnp);
        }
        
        if(c <= ' ')
            continue;
        
        else if(is_operation(c)){
            bignum * op_2 = pop();
            bignum * op_1 = pop();
            //printf("#op1:%s\tnum_of_digits: %ld\n#op2:%s\t\tnum_of_digits: %ld\n", op_1->digits, op_1->number_of_digits, op_2->digits, op_2->number_of_digits);    
            char * result = dispatch_ops(op_1, op_2, c);
            bignum * bnp = fill_big_num(result);
            push(bnp);
            free(op_1->digits);
            free(op_2->digits);
            free(op_1);
            free(op_2);
        }
        
        else if(c == 'p'){
	    if(peek())
            	printbn(peek());
	}
        
        else if(c == 'c'){ //free the stack
            free_stack();
            init_stack();
        }
        
        else if(c == 'q'){
            free_stack();
            break;
        }
        
        else{
            printf("illegal input: %c\n", c);
            exit(-1);
        }
    }
    free_stack();    
    return 0;
}

char * dispatch_ops(bignum * op1, bignum * op2, char operation){
    if(operation == '+'){
        if(negative(op1) && negative(op2)){
            //_a + _b = _(a + b)
            char * res = bnadd(op1, op2);
            res[0] = '_';
            return res;
        }
        
        else if(negative(op1)){
            //_a + b = b - a
            if(compare_bignums(op1, op2) <= 0)           
                return bnsub(op2, op1);
            else{ // op1 > op2
                char * res = bnsub(op1, op2);
                res[0] = '_';
                return res;
            }
        }
        
        else if(negative(op2)){
             //a + (_b) = a - b
            if(compare_bignums(op2, op1) <= 0)           
                return bnsub(op1, op2);
            else{ // op2 > op1
                char * res = bnsub(op2, op1);
                res[0] = '_';
                return res;
            }
        }
        
        else{
            return bnadd(op1, op2);
        }
    }
    
    else if(operation == '-'){
        if(negative(op1) && negative(op2)){
            //_a - _b = b - a
            if(compare_bignums(op1, op2) <= 0)           
                return bnsub(op2, op1);
            else{ // op1 > op2
                char * res = bnsub(op1, op2);
                res[0] = '_';
                return res;
            }
            //return bnsub(pos(b), pos(a))
        }
        
        else if(negative(op1)){
            //_a - b = _(a + b)
            char * res = bnadd(op1, op2);
            res[0] = '_';
            return res;
        }
        
        else if(negative(op2)){
            //a - (_b) = a + b
            return bnadd(op1, op2);
        }
        
        else{ // a - b
            if(compare_bignums(op2, op1) <= 0)           
                return bnsub(op1, op2);
            else{ // op2 > op1
                char * res = bnsub(op2, op1);
                res[0] = '_';
                return res;
            }
        }
    }
    

    else if(operation == '*'){
        bignum* first;
        bignum* second;
        bignum* result; 
        char* zerobn = (char*)malloc(3);
        zerobn[0] = '+'; zerobn[1] = '0'; zerobn[2] = '\0';
        result = fill_big_num(zerobn);
        
        char * op1_digits = (char *)malloc(op1->number_of_digits + 2);
        lngstrncpy(op1_digits, op1->digits, op1->number_of_digits + 1);
        op1_digits[op1->number_of_digits + 1] = '\0';
        
        char * op2_digits = (char *)malloc(op2->number_of_digits + 2);
        lngstrncpy(op2_digits, op2->digits, op2->number_of_digits + 1);
        op2_digits[op2->number_of_digits + 1] = '\0';
        
        char res_sign = (negative(op1) != negative(op2)) ? '_' : '+';
        if(compare_bignums(op2, op1) <= 0) { //op2 <= op1
            first = fill_big_num(op1_digits); // copy it so that the main loop can free op1,op2 later
            second = fill_big_num(op2_digits);
        }
        else{
            first = fill_big_num(op2_digits);         
            second = fill_big_num(op1_digits);
        }
        if (zero(second->digits)){
            return second->digits;
        }
        while(!one(second)){
            if (mod_2(second) == 1){
                bignum* result_temp = result;
                result = fill_big_num(bnadd(result,first));
                free(result_temp);
            }
            bignum* first_temp = first;
            bignum* second_temp = second;
            first = fill_big_num(bnadd(first, first));
            second = fill_big_num(div_by_2(second));
            free(first_temp);
            free(second_temp);
        }
        bignum* result_temp = result;
        result = fill_big_num(bnadd(result, first));
        free (result_temp);
        result->digits[0] = res_sign;
        return result->digits;
    }

    
    else{ //operation == '/'
        bignum* first;
        bignum* second;
        bignum* factor;
        bignum* result; 
        
        char* zerobn = (char*)malloc(3);
        zerobn[0] = '+'; zerobn[1] = '0'; zerobn[2] = '\0';
        result = fill_big_num(zerobn);
        
        char* onebn = (char*)malloc(3);
        onebn[0] = '+'; onebn[1] = '1'; onebn[2] = '\0';
        factor = fill_big_num(onebn);        
        
        char * op1_digits = (char *)malloc(op1->number_of_digits + 2);
        lngstrncpy(op1_digits, op1->digits, op1->number_of_digits + 1);
        op1_digits[op1->number_of_digits + 1] = '\0';
        
        char * op2_digits = (char *)malloc(op2->number_of_digits + 2);
        lngstrncpy(op2_digits, op2->digits, op2->number_of_digits + 1);
        op2_digits[op2->number_of_digits + 1] = '\0';
        
        char res_sign = (negative(op1) != negative(op2)) ? '_' : '+';
        if(compare_bignums(op2, op1) <= 0) { //op2 <= op1
            first = fill_big_num(op1_digits); // copy it so that the main loop can free op1,op2 later
            second = fill_big_num(op2_digits);
        }
        else{
            char* zero_b = (char*)malloc(3);
            zero_b[0] = '+'; zero_b[1] = '0'; zero_b[2] = '\0';
            return zero_b;
        }
        if (zero(second->digits)){
            printf("Divided by zero!!!");
            exit(-1);
        }
        divide(&first, &second, &result, &factor);
        result->digits[0] = res_sign;
        return result->digits;       
    }
}

void divide(bignum ** first, bignum ** second, bignum ** result, bignum ** factor){
    div_helper(first, second, result, factor);
    if(compare_bignums(*first, *second) >= 0){
        bignum * first_temp = *first;
        bignum * result_temp = *result;
        *first = fill_big_num(bnsub(*first, *second));
        *result = fill_big_num(bnadd(*result, *factor));
        free(first_temp);
        free(result_temp);   
    }
}
void div_helper(bignum ** first, bignum ** second, bignum ** result, bignum ** factor){
    if(compare_bignums(*first, *second) < 0){
        bignum * second_temp = *second;
        bignum * factor_temp = *factor;
        *second = fill_big_num(div_by_2(*second));
        *factor = fill_big_num(div_by_2(*factor));
        free(second_temp);
        free(factor_temp); 
        return;      
    }
    bignum * factor_temp = *factor;
    bignum * second_temp = *second;
    *factor = fill_big_num(bnadd(*factor, *factor));
    *second = fill_big_num(bnadd(*second, *second));
    free(factor_temp);
    free(second_temp);
    div_helper(first, second, result, factor);
    if(compare_bignums(*first, *second) >= 0){
       bignum * first_temp = *first;
       bignum * result_temp = *result;
       *first = fill_big_num(bnsub(*first, *second));
       *result = fill_big_num(bnadd(*result, *factor));
       free(first_temp);
       free(result_temp);
    }
    second_temp = *second;
    factor_temp = *factor;
    *second = fill_big_num(div_by_2(*second));
    *factor = fill_big_num(div_by_2(*factor));
    free(second_temp);
    free(factor_temp);
    return;
}

int compare_bignums(bignum * op1, bignum * op2){
    long n1 = op1->number_of_digits;
    long n2 = op2->number_of_digits;
    if(n1 != n2)
        return n1 < n2 ? -1 : 1;
    long i = 1;
    for( ; i <= n1 ; i++){
        if(op1->digits[i] == op2->digits[i])
            continue;
        else
            return op1->digits[i] < op2->digits[i] ? -1 : 1;
    }
    return 0;
}

long lngstrlen(const char *s){
  long n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

//+1234
//

void lngstrncpy(char *dest, const char *src, long n){

  while(n-- > 0 && (*dest++ = *src++) != 0)
    ;
  while(n-- > 0)
    *dest++ = 0;
  return;
}

int one(bignum* bn){
    return (bn->number_of_digits == 1 && bn->digits[1] == '1') ? TRUE : FALSE;
}

void printbn(bignum *bn){
    char * digits = bn->digits;
    if(digits[0] == '_')
        putchar('-');
    printf("%s\n", ++digits);
}