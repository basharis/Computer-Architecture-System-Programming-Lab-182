    global main

    extern printf
    extern scanf
    extern malloc
    extern calloc
    extern realloc
    extern free
    extern exit


;;;;;;;;;;;;;Macros;;;;;;;;;;;;;

%macro my_printf 2
    mov     rdi, %1                               ; String format
    mov     rsi, %2
    call    printf
%endmacro
%macro my_scanf 2
    mov     rdi, %1                               ; String format
    mov     rsi, %2                               ; Buffer to read into
    call    scanf
%endmacro
%macro my_malloc 1
    mov     rdi, %1
    call    malloc  
%endmacro
%macro my_free 1
    mov     rdi, %1
    call    free
%endmacro
%macro get_arr_element 2
    xor         %1, %1                            ; Long value
    lea         %1, [ARRAY + %2*QWORD_SIZE]       ; Extract current member address
    mov         qword %1, [%1]                    ; Extract long ptr
    mov         %1, [%1]                          ; Extract long val
%endmacro
%macro set_arr_element 2
    push        rdx
    lea         rdx, [ARRAY + %1*QWORD_SIZE]      ; Extract member address
    mov         qword rdx, [rdx]                  ; Extract long ptr
    mov         qword [rdx], %2                   ; Set it to the given long val
    pop         rdx
%endmacro

;;;;;;;;;;;;;End Macros;;;;;;;;;;;;;


section .data

    START_VM:       db "VM starting now...", 10 , 0
    DONE_VM:        db "VM has finished calculations. Printing memory dump:", 10, 0
    PTR_FMT:        db "%p", 10, 0
    LONG_FMT:       db "%ld", 0
    ASCII_NEWLINE:  db 10, 0
    ASCII_SPACE:    db 32, 0
    QWORD_SIZE:     equ 8
    ASCII_ZERO:     equ 48
    ASCII_COMMA:    equ 44
    ASCII_NINE:     equ 57

section .bss

    ARRAY: resq 8192                                ; Reserves 8192 qwords for SIC input


section .text

main:

    enter       0, 0
    my_printf   START_VM, 0                         ; Print a start message
    xor         rbx, rbx                            ; ARRAY index
    xor         r14, r14                            ; long ptr
    xor         r15, r15                            ; array ptr

.read_long:

    my_malloc   QWORD_SIZE                          ; Allocate qword for long  
    mov         r14, rax                            ; Save rax for later
    lea         r15, [ARRAY + rbx*QWORD_SIZE]       ; R15 gets the current address of ARRAY
    mov         qword [r15], r14                    ; Put the long ptr in ARRAY
    my_scanf    LONG_FMT, r14                       ; Read the long value into the allocated space
    inc         rbx                                 ; Increment ARRAY index
    cmp         rax, 1                              ; Scanf caught a match
    je          .read_long                          ; If so, continue reading
    dec         rbx
    lea         r12, [ARRAY + rbx*QWORD_SIZE]       ; Last space allocated is redundant
    mov         qword r13, [r12]                    ;
    my_free     r13                                 ; Free the last one
    mov         qword [r12], 0                      ; End ARRAY with 0
    xor         rbx, rbx                            ; ARRAY index
    jmp         .main_loop                          ; Continue


.main_loop:
    
    get_arr_element r13, rbx                        ; Extract first arg
    inc         rbx
    get_arr_element r14, rbx                        ; Extract second arg
    inc         rbx
    get_arr_element r15, rbx                        ; Extract third arg
    
    cmp         r13, 0                              ; If all of them are zero, finish
    jne         .sbn_cmd                            ; Otherwise, execute
    cmp         r14, 0
    jne         .sbn_cmd
    cmp         r15, 0
    jne         .sbn_cmd
    jmp         .print_array

.sbn_cmd:

    get_arr_element r8, r13                         ; Get M[M[r13]]
    get_arr_element r9, r14                         ; Get M[M[r14]]
    get_arr_element r10, r15                        ; Get M[M[r15]]
    sub             r8, r9                          ; M[M[r13]] - M[M[r14]]
    set_arr_element r13, r8                         ; M[M[r13]] = M[M[r13]] - M[M[r14]]
    cmp             r8, 0                           ; Check if negative
    jl              .branch                         ; If so, branch
    inc             rbx                             ; Otherwise, move to the next cmd
    jmp             .main_loop

.branch:
    mov             rbx, r15                        ; i <- M[i+2]
    jmp             .main_loop

.print_array:

    my_printf   DONE_VM, 0                          ; Print a done message
    xor         rbx, rbx                            ; ARRAY index

.print_loop:

    xor         r12, r12                            ; Will hold long ptr of ARRAY members
    xor         r13, r13                            ; Will hold long val of ARRAY members
    lea         r12, [ARRAY + rbx*QWORD_SIZE]       ; Put the next long ptr in r12
    cmp         qword [r12], 0                      ; Check if next member is 0
    je          .finished_printing                  ; If so, end printing
    mov         qword r12, [r12]
    mov         qword r13, [r12]                    ; Move the val of this cell to r13
    my_printf   LONG_FMT, r13                       ; Print the value of the current cell in ARRAY
    my_free     r12                                 ; Free the allocated long
    my_printf   ASCII_SPACE, 0                      ; After, print ' '
    inc         rbx                                 ; Increment ARRAY index
    jmp         .print_loop                         ; Continue printing

.finished_printing:

    my_printf   ASCII_NEWLINE, 0                    ; End with '\n'
    leave
