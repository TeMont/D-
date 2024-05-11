SC0: db '',10,'',00H
SC1: db '',10,'',00H
section .bss
extern GetStdHandle, WriteConsoleA, ReadConsoleA, ExitProcess

stdout_query equ -11
stdin_query equ -10
section .data
	stdout dw 0
	stdin dw 0
	bytesWritten dw 0
	bytesRead dw 0

section .bss
	OutputBuffer resb 20
	InputBuffer resb 256

section .text
global main
main:
;;	int let
	mov rdx, 10
	push rdx
	xor rdx, rdx
;;	/int let
;;	Output
	push QWORD [rsp + 0]
	pop rdx
	mov rax, rdx
	mov rsi, OutputBuffer
	call _itoa
	mov rdx, rsi
	call _printf
	mov rsi, OutputBuffer
	mov rdx, 20
	call _clearBuffer
;;	/Output
;;	Output
	mov rdx, SC0
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
;;	int let
	mov rdx, 20
	push rdx
	xor rdx, rdx
;;	/int let
;;	while loop
	label0:
	mov rdx, 's'
	cmp rdx, 0
	je label39
	mov rdx, 1
	jmp label38
	label39:
	mov rdx, 0
	label38:
	push rdx
	xor rdx, rdx
	mov rdx, 's'
	cmp rdx, 0
	je label41
	mov rdx, 1
	jmp label40
	label41:
	mov rdx, 0
	label40:
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label36
	mov rdx, 0
	jmp label37
	label36:
	mov rdx, 1
	label37:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	mov rdx, QWORD [rsp + 8]
	cmp rdx, 0
	je label45
	mov rdx, 1
	jmp label44
	label45:
	mov rdx, 0
	label44:
	push rdx
	xor rdx, rdx
	mov rdx, 0
	cmp rdx, 0
	je label47
	mov rdx, 1
	jmp label46
	label47:
	mov rdx, 0
	label46:
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	jg label42
	mov rdx, 0
	jmp label43
	label42:
	mov rdx, 1
	label43:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdi
	pop rdx
	cmp rdx, 0
	jle label34
	cmp rdi, 0
	jle label34
	mov rdx, 1
	jmp label35
	label34:
	mov rdx, 0
	label35:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	mov rdx, 1
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, 0
	jle label28
	cmp rdi, 0
	jle label28
	mov rdx, 1
	jmp label29
	label28:
	mov rdx, 0
	label29:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label1
;;	Output
	push QWORD [rsp + 8]
	pop rdx
	mov rax, rdx
	mov rsi, OutputBuffer
	call _itoa
	mov rdx, rsi
	call _printf
	mov rsi, OutputBuffer
	mov rdx, 20
	call _clearBuffer
;;	/Output
;;	Output
	mov rdx, SC1
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	push QWORD [rsp + 8]
	mov rdx, 1
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	sub rdx, rdi
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	push QWORD [rsp + 0]
	mov rdx, 2
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	sub rdx, rdi
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 0], rdx
	xor rdx, rdx
	jmp label0
	label1:
;;	/while loop

_printf:
	; INPUT:
	; RDX - string
	call _countStrLen
	mov r8, rcx
	mov rcx, stdout_query
	call GetStdHandle
	mov [rel stdout], rax
	mov rcx, [rel stdout]
	mov r9, bytesWritten
	xor r10, r10
	call WriteConsoleA
	ret

_scanf:
	; INPUT:
	; RDX - message
	; RSI - buffer for input
	; RAX - buffer size
	; OUTPUT:
	; RSI - buffer with user input
	push rax
	push rsi
	push rdx
	mov rdx, rax
	call _clearBuffer
	pop rdx
	call _printf
	mov rcx, stdin_query
	call GetStdHandle
	mov [rel stdin], rax
	mov rcx, [rel stdin]
	pop rdx
	pop r8
	mov r9, bytesRead
	call ReadConsoleA
	ret

_countStrLen:
	; INPUT:
	; RDX - string
	; OUTPUT:
	; RCX - string length
	xor rcx, rcx
	continue_count:
	mov al, byte [rdx + rcx]
	cmp al, 0
	je end_len_count
	inc rcx
	jmp continue_count
	end_len_count:
	ret

_itoa:
	; INPUT:
	; RSI - output string
	; RAX - integer
	; OUTPUT:
	; RSI - string
	push rsi
	push rax
	mov rdi, 1
	mov rcx, 1
	mov rbx, 10
	.get_divisor:
	xor rdx, rdx
	div rbx
	cmp rax, 0
	je ._after
	imul rcx, 10
	inc rdi
	jmp .get_divisor
	._after:
	pop rax
	push rdi
	.to_string:
	xor rdx, rdx
	div rcx
	add al, '0'
	mov [rsi], al
	inc rsi
	push rdx
	xor rdx, rdx
	mov rax, rcx
	mov rbx, 10
	div rbx
	mov rcx, rax
	pop rax
	cmp rcx, 0
	jg .to_string
	pop rdx
	pop rsi
	ret

_stoi:
	; INPUT:
	; RSI - buffer to convert
	; OUTPUT:
	; RDI - integer
	xor rdi, rdi
	mov rbx, 10
	xor rax, rax
	next_digit:
	movzx rdx, byte[rsi]
	test rdx, rdx
	jz done
	cmp rdx, 13
	je done
	cmp rdx, '0'
	jl error
	cmp rdx, '9'
	jg error
	imul rdi, rbx
	sub rdx, '0'
	add rdi, rdx
	inc rsi
	jmp next_digit
	error:
	mov rdx, WAR1
	call _printf
	mov rdx, 0
	done:
	mov rsi, rdx
	ret

_clearBuffer:
	; INPUT:
	; RSI - buffer to clear
	; RDX - buffer size
	clear:
	cmp rdx, 0
	je end
	cmp BYTE [rsi], 00H
	je end
	mov al, 00H
	mov [rsi], al
	inc rsi
	dec rdx
	jmp clear
	end:
	ret

WAR1: db 'Runtime Warning. Cannot Convert String To Integer. Assigned 0',7,10,00H
