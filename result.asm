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
	mov rdx, 3
	push rdx
	mov rdx, 1
	push rdx
	mov rdx, 0
	push rdx
	pop rdi
	pop rdx
	cmp rdx, rdi
	jg true0
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
	pop rdx
	cmp rdx, 0
	jle false2
	mov rdx, 1
	push rdx
	mov rdx, 1
	push rdx
	pop rdi
	pop rdx
	add rdx, rdi
	push rdx
	pop rdx
	mov [rsp + 0], rdx
	false2:
	push QWORD [rsp + 0]
	pop rcx
	call ExitProcess
