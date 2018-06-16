global bnadd, bnsub, div_by_2, mod_2
extern printf, malloc

;;;;;;;;;;;;;Macros;;;;;;;;;;;;;

%macro pushaq 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
%endmacro

%macro popaq 0
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro

%macro dprint 2
        pushaq
	move rdi %1
	move rsi %2
	call printf
        popaq
%endmacro 

;;;;;;;;;;;;;End Macros;;;;;;;;;;;;;

;;;;;;;;;;;;;Constants;;;;;;;;;;;;;

ASCII_ZERO: equ 48
BASE: equ 10
ASCII_ONE: equ 49
ASCII_UNDERSCORE: equ 95
ASCII_PLUS: equ 43

;;;;;;;;;;;;;End Constants;;;;;;;;;;;;;


section .text

;;;;;;;; ARITHMETIC HELPERS ;;;;;;;;;;;;;;;

mod_2:
    push rbp                    ; Save caller state
    mov rbp, rsp
    
    mov rax, rdi                ; Copy function args to registers: leftmost...
    mov qword rdi, [rax]        ; rdi = num of digits
    mov rsi, [rax + 8]          ; rsi = address of digits[0]

    add rsi, rdi                ; Move the cursor to digits[num_of_digits]
    xor rax, rax                ; Put zero in rax
    xor rbx, rbx
    mov byte al, [rsi]          ; Put digits[num_of_digits] in al
    sub al, ASCII_ZERO          ; Find the int value of the last digit
    ;;; Divide al / bl, div is stored in al, mod is stored in dl
    xor rdx, rdx
    mov bx, 2                   ; Move 2 to bl
    div bx                      ; Perform last_digit / 2
    cmp dl, 0                   ; Check if remainder is zero
    je .even_num
.odd_num:
    mov rax, 1
    jmp .finish
.even_num:
    mov rax, 0
.finish:
    pop rbp                     ; Restore caller state
    ret

div_by_2:
    push rbp                    ; Save caller state
    mov rbp, rsp

    mov rax, rdi                ; Copy function args to registers: leftmost...
    mov qword rdi, [rax]        ; rdi = num of digits
    mov rsi, [rax + 8]          ; rsi = address of digits[0]

    push rdi                    ; 
    push rsi                    ;
    add rdi, 2                  ; We need space for the sign and '\0'
    call malloc                 ; Allocate space for the quotient
    pop rsi                     ;
    pop rdi                     ;
    mov byte r11b, [rsi]        ; Take the sign char from the original
    mov byte [rax], r11b        ; Put it in the result
    mov r8, rax                 ; r8 holds the address of the result
    inc r8                      ; Move past the sign char (dest)
    push rax                    ; Save the place where the result begins
    inc rsi                     ; Move past the sign char (src)

    xor r15, r15                ; Init carry register
    mov rcx, rdi                ; Counter

    xor rdx, rdx                ; Init rdx to hold the remainder
.loop:
    xor rax, rax                ; Init rax for divides
    xor rbx, rbx
    xor rdx, rdx
    xor r9, r9
    
    mov byte al, [rsi]          ; Move the next digit into al
    sub al, ASCII_ZERO          ; Convert the digit from char to int
    add al, r15b                ; Add the carry
    mov bx, 2                   ; Divide by 2
    div bx                      ;
    mov r9b, al                 ; Start working with the quotioent
    add r9b, ASCII_ZERO         ; Convert it to char
    mov byte [r8], r9b          ; Write it to the result
    cmp dl, 0                   ; If remainder = 0
    je .no_carry                ; Jump to no_carry
    
.carry:
    mov r15b, 10                 ; Add 10 to the next digit
    jmp .continue
.no_carry:
    xor r15, r15                ; Reset the carry register

.continue:
    
    inc r8                      ; Next digit please (dest)
    inc rsi                     ; Next digit please (src)
    dec rcx                     ; Counter --
    cmp rcx, 0                  ; Loop if did not finish
    jnz .loop                   ;

.finish:
    mov byte [r8], 0            ; End the string with '\0'
    pop rax
    pop rbp                     ; Restore caller state
    ret


;;;;;;;;;;;;;;; ADD ;;;;;;;;;;;;;;;;;;;;;;;

bnadd:
    push rbp                    ; Save caller state
    mov rbp, rsp
    
    mov rax, rdi                ; Copy function args to registers: leftmost...
    mov rbx, rsi                ; Next argument...
    mov qword r8, [rax]         ; r8 = num of digits of arg_1
    mov rax, [rax + 8]          ; rax = address of first char of arg_1
    mov qword r9, [rbx]         ; r9 = num of digits of arg_2
    mov rbx, [rbx + 8]          ; rbx = address of first char of arg_2
    inc rax                     ; skip sign
    inc rbx                     ; skip sign

    push rax
    push rbx
    
    cmp r8, r9                  ; compare lengths of arg_1, arg_2
    jle .op1_is_min
    jg  .op2_is_min

.op1_is_min:
    mov qword rdi, r9           ; r8 <= r9, call malloc with r9
    add rdi, 3                  ; 1 for carry, 1 for the '\0',  1 for sign
    mov rcx, r8                 ; loop counter
    jmp .continue_init

.op2_is_min:
    mov qword rdi, r8           ; r9 <= r8, call malloc with r8
    add rdi, 3                  ; 1 for carray, 1 for '\0', 1 for sign
    mov rcx, r9                 ; loop counter

.continue_init:
    mov r11, rdi                ; r11 = requested size from malloc
    push rcx                    ; malloc is a registers destroyer
    push r8                     ; malloc is a registers destroyer
    push r9                     ; malloc is a registers destroyer
    call malloc                 ; allocate 'rdi' bytes
    pop r9                      ; Restore registers
    pop r8                      ; Restore registers
    pop rcx                     ; Restore registers
    mov r10, rax                ; r10 is a pointer to the start of the sum memory
    xor r15, r15                ; r15 is the carry register

.init_pointers:
    mov r14, r10                ; r14 = start of allocated block (will be returned in the end of the func)
    mov byte [r10], ASCII_PLUS  ; set sign to '+'
    inc r10                     ; goto carry cell
    mov byte [r10], ASCII_ZERO  ; set the carry additional cell to '0' result[1]
    add r10, r11                ; go to the end of r10 (r11 = requested size from malloc)
    sub r10, 2                  ; go to the last cell
    mov byte [r10], 0           ; end the string with '\0'
    dec r10                     ; r10 = last char in digits sum
    pop rbx                     ; Restore registers rbx = address of first char in arg_2       
    pop rax                     ; Restore registers rax = address of first char in arg_1   
    add rax, r8                 ; rax = address of last arg_1 char
    add rbx, r9                 ; rbx = address of last arg_2 char                 ; 
    dec rax                     ; now it really is 
    dec rbx                     ; now it really is               ; 

.loop:   
    mov r12b, [rax]             ; r12 = last char in arg_1
    mov r13b, [rbx]             ; r13 = last char in arg_2
    sub r12b, ASCII_ZERO        ; r12 = holds the digit in int repr
    sub r13b, ASCII_ZERO        ; r13 = holds the digit in int repr
    add r12b, r13b              ; r12 = r12 + r13
    add r12b, r15b              ; add last carry
    cmp r12b, BASE              ; r12 + r13 == 10 ?  
    jl  .no_carry
    jge .carry
    
.no_carry:
    xor r15b, r15b              ; reset carry
    jmp .continue_loop

.carry:
    mov byte r15b, 1            ; set carry flag
    sub r12b, BASE              ; restore digit

.continue_loop:
    add r12b, ASCII_ZERO        ; convert the digit to a char repr
    mov byte [r10], r12b        ; write the byte in r12b to sum memory
    dec r10             
    dec rax
    dec rbx
    dec rcx
    cmp rcx, 0
    jg .loop

.end_loop: 
    cmp r8, r9                  ; compare lengths of arg_1, arg_2
    je .done                    ; for an overflow (e.g 99 + 99)
    jl .set_op1_min
    jg .set_op2_min

.set_op1_min:                   ; r8 <= r9, we continue looping on rbx (arg2)
    sub r9, r8                  ; r9 = r9 - r8
    mov rcx, r9                 ; rcx = r9 - r8 (how many iterations left)
    mov rdx, rbx                ; rdx will be used as the pointer in continue_adding (who are we summing with the carry, arg1 or arg2?)
    jmp .continue_adding

.set_op2_min:                   ; r8 > r9, we continue looping on rax (arg1)
    sub r8, r9                  ; r8 = r8 - r9
    mov rcx, r8                 ; rcx = r8 - r9 (how many iterations left)
    mov rdx, rax                ; rdx will be used as the pointer in continue_adding (who are we summing with the carry, arg1 or arg2?)

.continue_adding:   
    cmp rcx, 0
    je .done
    mov r12b, [rdx]
    sub r12b, ASCII_ZERO        ; r12 = holds the digit in int repr
    add r12b, r15b              ; add the carry
    cmp r12b, BASE              ; r12 + carry == BASE ?  
    jl  .no_more_carry
    jge .more_carry
    
.more_carry:
    mov byte r15b, 1             ; set carry flag
    sub r12b, BASE               ; we had carry, restore original result
    add r12b, ASCII_ZERO         ; convert the digit to a char repr
    mov byte [r10], r12b         ; write to memory
    jmp .decrement_pointers

.no_more_carry:
    xor r15, r15                 ; reset carry
    add r12b, ASCII_ZERO         ; convert the digit to a char repr
    mov byte [r10], r12b         ; write to memory
    jmp .decrement_pointers
       
.decrement_pointers:
    dec r10                      ; dec sum pointer
    dec rcx                      ; dec left iterations
    dec rdx                      ; dec rdx (useless comment ever)
    jmp .continue_adding

.done:
    add r15b, ASCII_ZERO         ; convert the overflow digit to a char repr
    mov byte [r14 + 1], r15b     ; write it to memory (result[1])
    mov rax, r14                 ; return the sum ptr in rax (it was stored in r14 after calling malloc)
    
    pop rbp                      ; Restore caller state
    ret



;;;;;;;;;;;;;;; SUBTRACT ;;;;;;;;;;;;;;;;;;;;;;;

;assuming op1 >= op2
bnsub:
    push rbp                    ; Save caller state
    mov rbp, rsp
    
    mov rax, rdi                ; Copy function args to registers: leftmost...
    mov rbx, rsi                ; Next argument...
    mov qword r8, [rax]         ; r8 = num of digits of arg_1
    mov rax, [rax + 8]          ; rax = address of first char of arg_1
    mov qword r9, [rbx]         ; r9 = num of digits of arg_2
    mov rbx, [rbx + 8]          ; rbx = address of first char of arg_2
    inc rax                     ; skip sign
    inc rbx                     ; skip sign

    push rax
    push rbx
    
    cmp r8, r9                  ; compare lengths of arg_1, arg_2
    jle .op1_is_min
    jg  .op2_is_min

.op1_is_min:
    mov qword rdi, r9           ; r8 <= r9, call malloc with r9
    add rdi, 2                  ; 1 for the '\0',  1 for sign
    mov rcx, r8                 ; loop counter
    jmp .continue_init

.op2_is_min:
    mov qword rdi, r8           ; r9 <= r8, call malloc with r8
    add rdi, 2                  ; 1 for the '\0',  1 for sign
    mov rcx, r9                 ; loop counter

.continue_init:
    mov r11, rdi                ; r11 = requested size from malloc
    push rcx                    ; malloc is a registers destroyer
    push r8                     ; malloc is a registers destroyer
    push r9                     ; malloc is a registers destroyer
    call malloc                 ; allocate 'rdi' bytes
    pop r9                      ; Restore registers
    pop r8                      ; Restore registers
    pop rcx                     ; Restore registers
    mov r10, rax                ; r10 is a pointer to the start of the sum memory
    xor r15, r15                ; r15 is the borrow register

.init_pointers:
    mov r14, r10                ; r14 = start of allocated block (will be returned in the end of the func)
    mov byte [r10], ASCII_PLUS  ; set sign to '+'
    add r10, r11                ; go to the end of r10 (r11 = requested size from malloc)
    dec r10                     ; go to the last cell
    mov byte [r10], 0           ; end the string with '\0'
    dec r10                     ; r10 = last char in digits sum
    pop rbx                     ; Restore registers rbx = address of first char in arg_2       
    pop rax                     ; Restore registers rax = address of first char in arg_1   
    add rax, r8                 ; rax = address of last arg_1 char
    add rbx, r9                 ; rbx = address of last arg_2 char                 ; 
    dec rax                     ; now it really is 
    dec rbx                     ; now it really is               ; 

.loop:   
    mov r12b, [rax]             ; r12 = last char in arg_1
    mov r13b, [rbx]             ; r13 = last char in arg_2
    sub r12b, ASCII_ZERO        ; r12 = holds the digit in int repr
    sub r13b, ASCII_ZERO        ; r13 = holds the digit in int repr
    sub r12b, r13b              ; r12 = r12 + r13
    sub r12b, r15b              ; sub the borrow
    cmp r12b, 0                 ; r12 - r13 - borrow == 0 ?  
    jl .borrow
    jge  .no_borrow
    
.no_borrow:
    xor r15b, r15b              ; reset borrow
    jmp .continue_loop

.borrow:
    mov byte r15b, 1            ; set borrow flag
    add r12b, BASE              ; restore digit

.continue_loop:
    add r12b, ASCII_ZERO        ; convert the digit to a char repr
    mov byte [r10], r12b        ; write the byte in r12b to sum memory
    dec r10             
    dec rax
    dec rbx
    dec rcx
    cmp rcx, 0
    jg .loop

.end_loop: 
    cmp r8, r9                  ; compare lengths of arg_1, arg_2 (r8 >= r9)
    je .done                    ; 

.set_op2_min:                   ; r8 > r9, we continue looping on rax (arg1)
    sub r8, r9                  ; r8 = r8 - r9
    mov rcx, r8                 ; rcx = r8 - r9 (how many iterations left)
    mov rdx, rax                ; rdx will be used as the pointer in continue_adding (who are we summing with the carry, arg1 or arg2?)

.continue_adding:   
    cmp rcx, 0
    je .done
    mov r12b, [rdx]
    sub r12b, ASCII_ZERO        ; r12 = holds the digit in int repr
    sub r12b, r15b              ; sub the borrow
    cmp r12b, 0                 ; r12b - borrow == 0 ?  
    jl  .more_carry
    jge .no_more_carry
    
.more_carry:
    mov byte r15b, 1             ; set carry flag
    add r12b, BASE               ; we had borrow, restore original result
    add r12b, ASCII_ZERO         ; convert the digit to a char repr
    mov byte [r10], r12b         ; write to memory
    jmp .decrement_pointers

.no_more_carry:
    xor r15, r15                 ; reset carry
    add r12b, ASCII_ZERO         ; convert the digit to a char repr
    mov byte [r10], r12b         ; write to memory
    jmp .decrement_pointers
       
.decrement_pointers:
    dec r10                      ; dec sum pointer
    dec rcx                      ; dec left iterations
    dec rdx                      ; dec rdx (useless comment ever)
    jmp .continue_adding

.done:
    mov rax, r14                 ; return the sum ptr in rax (it was stored in r14 after calling malloc)
    
    pop rbp                      ; Restore caller state
    ret