SC0: db 'str', 00H
extern GetStdHandle, WriteConsoleA, ExitProcess

stdout_query equ -11
section .data
	stdout dw 0
	bytesWritten dw 0

section .bss
	buffer resb 20

section .text
global main
main:
;;	int let
	mov rdx, 1
	push rdx
	xor rdx, rdx
;;	/int let
;;	bool let
	push QWORD [rsp + 0]
	mov rdx, 1
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	jg label0
	mov rdx, 0
	jmp label1
	label0:
	mov rdx, 1
	label1:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	jle label2
	mov rdx, 1
	jmp label3
	label2:
	mov rdx, 0
	label3:
	push rdx
	xor rdx, rdx
;;	/bool let
;;	Output
	push QWORD [rsp + 8]
	pop rdx
	mov rax, rdx
	mov rsi, buffer
	call _itoa
	mov rdx, rsi
	call _printf
;;	/Output
;;	Output
	mov rdx, SC0
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
;;	Output
	mov rdx, 1
	push rdx
	xor rdx, rdx
	pop rdx
	mov rax, rdx
	mov rsi, buffer
	call _itoa
	mov rdx, rsi
	call _printf
;;	/Output
;;	Output
	push QWORD [rsp + 0]
	pop rdx
	mov rax, rdx
	mov rsi, buffer
	call _itoa
	mov rdx, rsi
	call _printf
;;	/Output
;;	Output
	mov rdx, 666
	push rdx
	xor rdx, rdx
	pop rdx
	mov rax, rdx
	mov rsi, buffer
	call _itoa
	mov rdx, rsi
	call _printf
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
	call _count_str_len
	mov r8, rcx
	mov rcx, stdout_query
	call GetStdHandle
	mov [rel stdout], rax
	mov rcx, [rel stdout]
	mov r9, bytesWritten
	xor r10, r10
	call WriteConsoleA
	ret

_count_str_len:
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
