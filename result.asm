section .text
global _start
extern ExitProcess
_start:
	push rdx
	push rdx
	mov rdx, 2
	push rdx
	mov rdx, 2
	push rdx
	pop rdi
	pop rdx
	imul rdx, rdi
	push rdx
	mov rdx, 2
	push rdx
	pop rdi
	pop rdx
	add rdx, rdi
	push rdx
	pop rdx
	mov [rsp + 8], rdx
	push QWORD [rsp + 8]
	mov rdx, 2
	push rdx
	pop rdi
	pop rdx
	mov rax, rdx
	mov rdx, 0
	idiv rdi
	mov rdx, rax
	push rdx
	mov rdx, 9
	push rdx
	mov rdx, 3
	push rdx
	pop rdi
	pop rdx
	imul rdx, rdi
	push rdx
	pop rdi
	pop rdx
	sub rdx, rdi
	push rdx
	pop rdx
	mov [rsp + 0], rdx
	push QWORD [rsp + 0]
	pop rcx
	call ExitProcess
