section .text
global _start
extern ExitProcess
_start:
	push rdx
	push rdx
	mov rdx, 200
	push rdx
	pop rdx
	mov [rsp + 8], rdx
	push QWORD [rsp + 8]
	mov rdx, 100
	push rdx
	pop rdi
	pop rdx
	mov rax, rdx
	mov rdx, 0
	idiv rdi
	mov rdx, rax
	push rdx
	pop rdx
	mov [rsp + 0], rdx
	push QWORD [rsp + 0]
	pop rdx
	mov [rsp + 8], rdx
	push QWORD [rsp + 8]
	pop rcx
	call ExitProcess
