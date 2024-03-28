section .text
global _start
extern ExitProcess
_start:
	push rdx
	push rdx
	mov rdx, 15
	push rdx
	pop rdx
	mov [rsp + 0], rdx
	push QWORD [rsp + 0]
	pop rdx
	mov [rsp + 8], rdx
	push QWORD [rsp + 8]
	pop rcx
	call ExitProcess
