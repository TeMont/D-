section .text
global _start
extern ExitProcess
_start:
;;	int let
	push rdx
;;	/int let
	mov rdx, 1
	push rdx
	pop rdx
	mov [rsp + 0], rdx
;;	return
	push QWORD [rsp + 0]
	pop rcx
	call ExitProcess
;;	/return
