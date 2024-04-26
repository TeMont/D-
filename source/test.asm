SC0: db 'Enter Your Name: ',00H
SC2: db 'Your Name Is ',00H
SC3: db '',10,'',00H
SC4: db 'Enter Your Age: ',00H
SC5: db 'Your Age Is ',00H
section .bss
	SC1 resb 256
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
;;	str let
;;	Input
	mov rdx, SC0
	push rdx
	xor rdx, rdx
	pop rdx
	mov rsi, InputBuffer
	mov rax, 256
	call _scanf
	mov rdi, SC1
	mov rcx, 256
	rep movsb
	mov rdx, SC1
	call _countStrLen
	cmp byte [rdx+rcx-1], 10
	je label1
	jmp label2
	label1:
	mov byte [rdx+rcx-1], 00H
	label2:
	cmp byte [rdx+rcx-2], 13
	je label4
	jmp label5
	label4:
	mov byte [rdx+rcx-2], 00H
	label5:
	push rdx
	mov rsi, OutputBuffer
	mov rdx, 20
	call _clearBuffer
	mov rsi, InputBuffer
	mov rdx, 256
	call _clearBuffer
;;	/Input
;;	/str let
;;	Output
	mov rdx, SC2
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
;;	Output
	push QWORD [rsp + 0]
	pop rdx
	call _printf
;;	/Output
;;	Output
	mov rdx, SC3
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
;;	int let
;;	Input
	mov rdx, SC4
	push rdx
	xor rdx, rdx
	pop rdx
	mov rsi, InputBuffer
	mov rax, 256
	call _scanf
	call _stoi
	push rdi
	mov rsi, OutputBuffer
	mov rdx, 20
	call _clearBuffer
	mov rsi, InputBuffer
	mov rdx, 256
	call _clearBuffer
;;	/Input
;;	/int let
;;	Output
	mov rdx, SC5
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
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
;;	return
	mov rdx, 1
	push rdx
	xor rdx, rdx
	pop rcx
	call ExitProcess
;;	/return

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
	mov rdx, ERR1
	call _printf
	call ExitProcess
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

ERR1: db 'Runtime Error. Cannot Convert String To Integer',7,00H
