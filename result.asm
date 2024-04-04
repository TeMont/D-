section .text
global _start
extern ExitProcess
_start:
	push rdx
	mov rdx, '6 / 2 - 9 * 3'
	push rdx
	push QWORD [rsp + 8]
	pop rcx
	call ExitProcess