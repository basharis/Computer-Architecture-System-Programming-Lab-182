section .data						; data section, read-write
        an:    DQ 0					; this is a temporary var
		
section .text						; our code is always in the .text section
        global do_Str				; makes the function appear in global scope
        extern printf				; tell linker that printf is defined elsewhere 							; (not used in the program)

do_Str:								; functions are defined as labels
        push    rbp					; save Base Pointer (bp) original value
        mov     rbp, rsp			; use base pointer to access stack contents
        mov		rcx, rdi			; get function argument

;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE STARTS HERE ;;;;;;;;;;;;;;;; 

	mov	qword [an], 0		; initialize answer
	cmp byte [rcx], 0		; check if byte pointed to is zero, for the first byte (empty string)
	je next_char_done		; if the first byte is zero - end loop
	
	next_char:
		cmp byte [rcx], 'a'		; check if the char is a lower case letter
		jl not_lower_case		; 
		cmp byte [rcx], 'z'		;
		jg not_lower_case		;
		mov bl, byte [rcx]		; move the lower case char to bl since it is a byte
		sub bl, 0x20			; subtract 26 from this char
		mov byte [rcx], bl		; move it back to the string
		not_lower_case:
		
		cmp byte [rcx], 'A'		; if the char was lower case OR upper case, now it is upper case
		jl not_a_letter			; 
		cmp byte [rcx], 'Z'		;
		jl continue				;
		
		not_a_letter:
		cmp byte [rcx], '('		; check if the char is '('
		jne not_open_par    	; if not, continue
		mov byte [rcx], '<'		; if so, change it to '<'
		not_open_par:
		
		cmp byte [rcx], ')'		; check if the char is ')'
		jne not_close_par    	; if not, continue
		mov byte [rcx], '>'		; if so, change it to '<'
		not_close_par:
		
		all_special_chars:
		inc qword [an]			; this is a special character, increase the counter
		
		continue:
		inc rcx					; increment pointer
		cmp byte [rcx], 0		; check if byte pointed to is zero
		jnz next_char			; keep looping until it is null terminated


;;;;;;;;;;;;;;;; FUNCTION EFFECTIVE CODE ENDS HERE ;;;;;;;;;;;;;;;; 
	next_char_done:
	
        mov     rax,[an]         ; return an (returned values are in rax)
        mov     rsp, rbp
        pop     rbp
        ret