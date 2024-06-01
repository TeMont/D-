SC0: db 'Enter First Num: ',00H
SC1: db 'Enter Action Symbol: ',00H
SC2: db 'Enter Second Num: ',00H
SC3: db 'You Entered Incorrect Symbol',00H
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
	mov rcx, 4
	clear_zeroes:
	dec rsi
	mov al, byte [rsi]
	cmp al, '0'
	jne finish_clearing
	mov byte [rsi], 00h
	loop clear_zeroes
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
	jl done
	cmp rdx, '9'
	jg done
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
	jl done
	cmp rdx, '9'
	jg done
	imul rdi, rbx
	sub rdx, '0'
	add rdi, rdx
	inc rsi
	jmp next_digit
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

	_stof:
	; INPUT:
	; RSI - buffer to convert
	; OUTPUT:
	; XMM0 - integer
	call _stoi
	cmp byte [rsi], '.'
	jne finish
	inc rsi
	push rdi
	mov rdx, rsi
	call _countStrLen
	push rcx
	call _stoi
	cvtsi2ss xmm0, rdi
	pop rcx
	mov rdx,__?float32?__(0.1)
	movq xmm1, rdx
	divide:
	mulss xmm0, xmm1
	loop divide
	pop rdx
	cvtsi2ss xmm1, rdx
	addss xmm0, xmm1
	ret
	finish:
	cvtsi2ss xmm0, rdi
	ret
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
;;	Input
	mov rdx, SC0
	push rdx
	xor rdx, rdx
	pop rdx
	mov rsi, InputBuffer
	mov rax, 256
	call _scanf
	mov rdx, rsi
	call _countStrLen
	cmp byte [rdx+rcx-1], 10
	je label0
	jmp label1
	label0:
	mov byte [rdx+rcx-1], 00H
	label1:
	cmp byte [rdx+rcx-2], 13
	je label2
	jmp label3
	label2:
	mov byte [rdx+rcx-2], 00H
	label3:
	call _stof
	movq rdx, xmm0
	push rdx
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
	mov rdx, SC1
	push rdx
	xor rdx, rdx
	pop rdx
	mov rsi, InputBuffer
	mov rax, 256
	call _scanf
	mov rdx, rsi
	call _countStrLen
	cmp byte [rdx+rcx-1], 10
	je label4
	jmp label5
	label4:
	mov byte [rdx+rcx-1], 00H
	label5:
	cmp byte [rdx+rcx-2], 13
	je label6
	jmp label7
	label6:
	mov byte [rdx+rcx-2], 00H
	label7:
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
	mov rdx, rsi
	call _countStrLen
	cmp byte [rdx+rcx-1], 10
	je label8
	jmp label9
	label8:
	mov byte [rdx+rcx-1], 00H
	label9:
	cmp byte [rdx+rcx-2], 13
	je label10
	jmp label11
	label10:
	mov byte [rdx+rcx-2], 00H
	label11:
	call _stof
	movq rdx, xmm0
	push rdx
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
	mov rdx, QWORD [rsp + 0]
	push rdx
	mov rdx, '+'
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label13
	mov rdx, 0
	jmp label14
	label13:
	mov rdx, 1
	label14:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label12
	mov rdx, QWORD [rsp + 24]
	push rdx
	mov rdx, QWORD [rsp + 24]
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
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	xor rdx, rdx
;;	/if
	jmp label17
	label12:
;;	elif
	mov rdx, QWORD [rsp + 0]
	push rdx
	mov rdx, '-'
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label19
	mov rdx, 0
	jmp label20
	label19:
	mov rdx, 1
	label20:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label18
	mov rdx, QWORD [rsp + 24]
	push rdx
	mov rdx, QWORD [rsp + 24]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	subss xmm0, xmm1
	movq rdx, xmm0
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	xor rdx, rdx
	jmp label17
;;	/elif
	label18:
;;	elif
	mov rdx, QWORD [rsp + 0]
	push rdx
	mov rdx, '*'
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label24
	mov rdx, 0
	jmp label25
	label24:
	mov rdx, 1
	label25:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label23
	mov rdx, QWORD [rsp + 24]
	push rdx
	mov rdx, QWORD [rsp + 24]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	mulss xmm0, xmm1
	movq rdx, xmm0
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	xor rdx, rdx
	jmp label17
;;	/elif
	label23:
;;	elif
	mov rdx, QWORD [rsp + 0]
	push rdx
	mov rdx, '/'
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	je label29
	mov rdx, 0
	jmp label30
	label29:
	mov rdx, 1
	label30:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label28
	mov rdx, QWORD [rsp + 24]
	push rdx
	mov rdx, QWORD [rsp + 24]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	divss xmm0, xmm1
	movq rdx, xmm0
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	mov [rsp + 8], rdx
	xor rdx, rdx
	xor rdx, rdx
	jmp label17
;;	/elif
	label28:
;;	else
;;	Output
	mov rdx, SC3
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
	label17:
	xor rdx, rdx
;;	Output
	mov rdx, QWORD [rsp + 8]
	push rdx
	pop rdx
	movq xmm0, rdx
	mov rsi, OutputBuffer
	call _ftoa
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
