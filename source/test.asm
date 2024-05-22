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
	push rax
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
	pop rax
	ret

_ftoa:
	; INPUT:
	; RSI - output string
	; XMM0 - float value
	; OUTPUT:
	; RSI - updated string
	push rsi
	mov rdx,__?float32?__(10.0)
	movq xmm1, rdx
	mov rcx, 4
	float_mul:
	mulss xmm0, xmm1
	loop float_mul
	roundss xmm0, xmm0, 0
	cvttss2si rdx, xmm0
	cvtsi2ss xmm0, rdx
	mov rcx, 4
	float_div:
	mov rdx,__?float32?__(0.1)
	movq xmm1, rdx
	mulss xmm0, xmm1
	loop float_div
	cvttss2si rax, xmm0
	call _itoa
	increaseBuffer:
	cmp byte [rsi], 00h
	je endIncreasing
	inc rsi
	jmp increaseBuffer
	endIncreasing:
	mov byte [rsi], '.'
	inc rsi
	cvtsi2ss xmm1, rax
	subss xmm0, xmm1
	mov r10, 4
	mov rdx, __?float32?__(0.0)
	movq xmm1, rdx
	comiss xmm0, xmm1
	jb negative_float
	jmp convert_fraction
	negative_float:
	mov rdx, __?float32?__(-1.0)
	movq xmm1, rdx
	mulss xmm0, xmm1
	convert_fraction:
	mov rdx, __?float32?__(10.0)
	movq xmm1, rdx
	mulss xmm0, xmm1
	cvttss2si rax, xmm0
	call _itoa
	cvtsi2ss xmm1, rax
	subss xmm0, xmm1
	inc rsi
	dec r10
	cmp r10, 0
	jl end_convert
	jmp convert_fraction
	end_convert:
	clear_zeroes:
	dec rsi
	mov al, byte [rsi]
	cmp al, '0'
	jne finish_clearing
	mov byte [rsi], 00h
	jmp clear_zeroes
	finish_clearing:
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
	mov rdx,__?float32?__(2.1)
	push rdx
	xor rdx, rdx
;;	/let
;;	let
	mov rdx, 2
	mov rdx, rdx
	cvtsi2ss xmm0, rdx
	movq rdx, xmm0
	push rdx
	xor rdx, rdx
;;	/let
;;	let
	push rdx
;;	/let
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	addss xmm0, xmm1
	movq rdx, xmm0
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	mov rdx, 4
	mov rdx, rdx
	cvtsi2ss xmm0, rdx
	movq rdx, xmm0
	push rdx
	xor rdx, rdx
	mov rdx, QWORD [rsp + 32]
	push rdx
	mov rdx, QWORD [rsp + 32]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	addss xmm0, xmm1
	movq rdx, xmm0
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	je label4
	mov rdx, 0
	jmp label5
	label4:
	mov rdx, 1
	label5:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov rdx, rdx
	cmp rdx, 0
	je label7
	mov rdx, 1
	jmp label6
	label7:
	mov rdx, 0
	label6:
	push rdx
	xor rdx, rdx
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
;;	Output
	mov rdx, QWORD [rsp + 8]
	push rdx
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
	mov rdx, 2
	push rdx
	xor rdx, rdx
	pop rcx
	call ExitProcess
;;	/return
