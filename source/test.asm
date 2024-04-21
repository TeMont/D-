SC0: db 'a', 00H
extern GetStdHandle, WriteConsoleA, ExitProcess

stdout_query equ -11
section .data
	stdout dw 0
	bytesWritten dw 0

section .text
global main
main:
;;	str let
	mov rdx, SC0
	push rdx
	xor rdx, rdx
;;	/str let
;;	str let
	push QWORD [rsp + 0]
;;	/str let
;;	return
	push QWORD [rsp + 0]
	pop rcx
	call ExitProcess
;;	/return

_printf:
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
	xor rcx, rcx
	continue_count:
	mov al, byte [rdx + rcx]
	cmp al, 0
	je end_len_count
	inc rcx
	jmp continue_count
	end_len_count:
	ret

