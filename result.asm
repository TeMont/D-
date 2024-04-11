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
	mov rdx, 1
	push rdx
	pop rdi
	pop rdx
	cmp rdx, 0
	jle false1
	cmp rdi, 0
	jle false1
	mov rdx, 1
	jmp end1
	false1:
	mov rdx, 0
	end1:
	push rdx
	mov rdx, 0
	push rdx
	pop rdi
	pop rdx
	cmp rdx, 0
	je true0
	cmp rdi, 0
	je true0
	mov rdx, 0
	jmp end2
	true0:
	mov rdx, 1
	end2:
	push rdx
	mov rdx, 0
	push rdx
	pop rdi
	pop rdx
	cmp rdx, 0
	je true1
	cmp rdi, 0
	je true1
	mov rdx, 0
	jmp end3
	true1:
	mov rdx, 1
	end3:
	push rdx
	pop rdx
	cmp rdx, 0
	jle false2
	mov rdx, 1
	jmp end4
	false2:
	mov rdx, 0
	end4:
	mov [rsp + 8], rdx
	push QWORD [rsp + 8]
	pop rcx
	call ExitProcess
