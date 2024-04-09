section .text
global _start
extern ExitProcess
_start:
	push rdx
	pop rdx
	cmp rdx, 0
	jle false0
	mov rdx, 1
	jmp end0
	false0:
	mov rdx, 0
	end0:
	push rdx
	mov rdx, 2
	push rdx
	push QWORD [rsp + 0]
	mov rdx, 2
	push rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	jge true0
	mov rdx, 0
	jmp end1
	true0:
	mov rdx, 1
	end1:
	push rdx
	pop rdx
	cmp rdx, 0
	jle false1
	mov rdx, 1
	jmp end2
	false1:
	mov rdx, 0
	end2:
	mov [rsp + 8], rdx
	push QWORD [rsp + 8]
	pop rcx
	call ExitProcess
