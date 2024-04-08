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
	mov rdx, 123
	push rdx
	pop rdx
	cmp rdx, 0
	jle false1
	mov rdx, 1
	jmp end1
	false1:
	mov rdx, 0
	end1:
	mov [rsp + 0], rdx
	mov rdx, ''
	push rdx
	pop rdx
	cmp rdx, 0
	jle false2
	mov rdx, 1
	jmp end2
	false2:
	mov rdx, 0
	end2:
	mov [rsp + 0], rdx
	mov rdx, '213'
	push rdx
	pop rdx
	cmp rdx, 0
	jle false3
	mov rdx, 1
	jmp end3
	false3:
	mov rdx, 0
	end3:
	mov [rsp + 0], rdx
	push QWORD [rsp + 0]
	pop rcx
	call ExitProcess
