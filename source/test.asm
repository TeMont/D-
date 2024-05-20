SC0: db 'a is greater than b',10,'',00H
SC1: db 'a is less than b',10,'',00H
SC2: db 'a is not less than b',10,'',00H
SC3: db 'a is greater than or equal to b',10,'',00H
SC4: db 'a is less than or equal to b',10,'',00H
SC5: db 'a is equal to b',10,'',00H
SC6: db 'a is not equal to b',10,'',00H
SC7: db 'a is not equal to b',10,'',00H
SC8: db 'a is positive number',10,'',00H
SC9: db 'b is positive number',10,'',00H
SC10: db 'a is a negative number',10,'',00H
SC11: db 'a is not a negative number',10,'',00H
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
	mov rdx,__?float32?__(5.2)
	push rdx
	xor rdx, rdx
;;	/let
;;	let
	mov rdx,__?float32?__(3.8)
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
	pop rdx
	mov [rsp + 0], rdx
	xor rdx, rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
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
	mov [rsp + 0], rdx
	xor rdx, rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
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
	mov [rsp + 0], rdx
	xor rdx, rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
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
	mov [rsp + 0], rdx
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	ja label9
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
;;	Output
	mov rdx, SC0
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	label8:
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	jb label12
	mov rdx, 0
	jmp label13
	label12:
	mov rdx, 1
	label13:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label11
;;	Output
	mov rdx, SC1
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	jmp label14
	label11:
;;	else
;;	Output
	mov rdx, SC2
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/else
	label14:
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	jae label16
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
;;	Output
	mov rdx, SC3
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	label15:
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	jbe label19
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
;;	Output
	mov rdx, SC4
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	label18:
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	je label22
	mov rdx, 0
	jmp label23
	label22:
	mov rdx, 1
	label23:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label21
;;	Output
	mov rdx, SC5
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	jmp label24
	label21:
;;	else
;;	Output
	mov rdx, SC6
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/else
	label24:
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx, QWORD [rsp + 16]
	push rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	jne label26
	mov rdx, 0
	jmp label27
	label26:
	mov rdx, 1
	label27:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label25
;;	Output
	mov rdx, SC7
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	label25:
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx,__?float32?__(0.0)
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	ja label29
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
;;	Output
	mov rdx, SC8
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	label28:
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 8]
	push rdx
	mov rdx,__?float32?__(0.0)
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	ja label32
	mov rdx, 0
	jmp label33
	label32:
	mov rdx, 1
	label33:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label31
;;	Output
	mov rdx, SC9
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	label31:
	xor rdx, rdx
;;	if
	mov rdx, QWORD [rsp + 16]
	push rdx
	mov rdx,__?float32?__(0.0)
	push rdx
	xor rdx, rdx
	pop rdi
	pop rdx
	movq xmm0, rdx
	movq xmm1, rdi
	comiss xmm0, xmm1
	jb label35
	mov rdx, 0
	jmp label36
	label35:
	mov rdx, 1
	label36:
	push rdx
	xor rdx, rdx
	xor rdi, rdi
	pop rdx
	cmp rdx, 0
	je label34
;;	Output
	mov rdx, SC10
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/if
	jmp label37
	label34:
;;	else
;;	Output
	mov rdx, SC11
	push rdx
	xor rdx, rdx
	pop rdx
	call _printf
;;	/Output
	xor rdx, rdx
;;	/else
	label37:
	xor rdx, rdx
;;	return
	mov rdx, 0
	push rdx
	xor rdx, rdx
	pop rcx
	call ExitProcess
;;	/return
