SC0: db 'Enter First Num: ',00H
SC1: db 'Enter Action Symbol: ',00H
SC2: db 'Enter Second Num: ',00H
SC3: db 'Enter Third Num: ',00H
SC4: db 'You Entered Incorrect Symbol',00H
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
	xor r9, r9
	test rax, rax
	jns .positive
	neg rax
	mov r9, 1
	jmp .start
	.positive:
	mov r9, 0
	.start:
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
	test r9, 1
	jz .to_string
	mov byte [rsi], '-'
	inc rsi
	xor r9, r9
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
	mov rcx, 1
	movzx rdx, byte[rsi]
	cmp rdx, '-'
	je negative
	cmp rdx, '+'
	je positive
	cmp rdx, '0'
	jl error
	cmp rdx, '9'
	jg error
	jmp next_digit
	positive:
	inc rsi
	jmp next_digit
	negative:
	inc rsi
	mov rcx, 0
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
	mov rdi, 0
	done:
	cmp rcx, 0
	je apply_negative
	ret
	apply_negative:
	neg rdi
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
global main
main:
;;	let
	push rdx
;;	/let
;;	let
	push rdx
;;	/let
;;	let
	push rdx
;;	/let
;;	let
	push rdx
;;	/let
;;	let
	push rdx
;;	/let
;;	Input
	mov rdx, SC0
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
	pop rdx
	mov [rsp + 32], rdx
	xor rdx, rdx
;;	Input
	mov rdx, SC1
	push rdx
	xor rdx, rdx
	pop rdx
	mov rsi, InputBuffer
	mov rax, 256
	call _scanf
	movzx rdx, byte [rsi]
	push rdx
	mov rsi, OutputBuffer
	mov rdx, 20
	call _clearBuffer
	mov rsi, InputBuffer
	mov rdx, 256
	call _clearBuffer
;;	/Input
	pop rdx
	mov [rsp + 0], rdx
	xor rdx, rdx
;;	Input
	mov rdx, SC2
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
	pop rdx
	mov [rsp + 24], rdx
	xor rdx, rdx
;;	Input
	mov rdx, SC3
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
	pop rdx
	mov [rsp + 16], rdx
	xor rdx, rdx
;;	if
	push QWORD [rsp + 0]
	mov rdx, '+'
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label1
	mov rdx, 0
	jmp label2
	label1:
	mov rdx, 1
	label2:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label0
	push QWORD [rsp + 32]
	push QWORD [rsp + 32]
	pop rdi
	pop rdx
	add rdx, rdi
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	push QWORD [rsp + 24]
	pop rdi
	pop rdx
	add rdx, rdi
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	xor rdx, rdx
;;	/if
	jmp label7
	label0:
;;	elif
	push QWORD [rsp + 0]
	mov rdx, '-'
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label9
	mov rdx, 0
	jmp label10
	label9:
	mov rdx, 1
	label10:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label8
	push QWORD [rsp + 32]
	push QWORD [rsp + 32]
	pop rdi
	pop rdx
	sub rdx, rdi
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	push QWORD [rsp + 24]
	pop rdi
	pop rdx
	sub rdx, rdi
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	xor rdx, rdx
	jmp label7
;;	/elif
	label8:
;;	elif
	push QWORD [rsp + 0]
	mov rdx, '*'
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label16
	mov rdx, 0
	jmp label17
	label16:
	mov rdx, 1
	label17:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label15
	push QWORD [rsp + 32]
	push QWORD [rsp + 32]
	pop rdi
	pop rdx
	imul rdx, rdi
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	push QWORD [rsp + 24]
	pop rdi
	pop rdx
	imul rdx, rdi
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	xor rdx, rdx
	jmp label7
;;	/elif
	label15:
;;	elif
	push QWORD [rsp + 0]
	mov rdx, '/'
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label23
	mov rdx, 0
	jmp label24
	label23:
	mov rdx, 1
	label24:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label22
	push QWORD [rsp + 32]
	push QWORD [rsp + 32]
	pop rdi
	pop rdx
	mov rax, rdx
	xor rdx, rdx
	cqo
	idiv rdi
	mov rdx, rax
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	push QWORD [rsp + 24]
	pop rdi
	pop rdx
	mov rax, rdx
	xor rdx, rdx
	cqo
	idiv rdi
	mov rdx, rax
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	xor rdx, rdx
	jmp label7
;;	/elif
	label22:
;;	else
;;	Output
	mov rdx, SC4
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
;;	return
	mov rdx, 1
	push rdx
	xor rdx, rdx
	pop rcx
	call ExitProcess
;;	/return
	xor rdx, rdx
;;	/else
	xor rdx, rdx
	xor rdx, rdx
	xor rdx, rdx
	label7:
	xor rdx, rdx
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
;;	return
	mov rdx, 0
	push rdx
	xor rdx, rdx
	pop rcx
	call ExitProcess
;;	/return
