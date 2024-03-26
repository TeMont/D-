section .text
global _start
extern ExitProcess
_start:
	mov rdi, 20
	push rdi
	mov rdi, 26
	push rdi
	push QWORD [rsp + 0]
	pop rcx
	call ExitProcess
