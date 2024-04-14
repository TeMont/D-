section .text
global _start
extern ExitProcess
_start:
;;	if
	mov rdx, 0
	push rdx
	pop rdx
	cmp rdx, 0
	jle label0
;;	return
	mov rdx, 1
	push rdx
	pop rcx
	call ExitProcess
;;	/return
;;	/if
	jmp label1
	label0:
;;	elif
	mov rdx, 0
	push rdx
	pop rdx
	cmp rdx, 0
	jle label2
;;	return
	mov rdx, 2
	push rdx
	pop rcx
	call ExitProcess
;;	/return
;;	/elif
label2:
;;	elif
	mov rdx, 1
	push rdx
	pop rdx
	cmp rdx, 0
	jle label3
;;	return
	mov rdx, 4
	push rdx
	pop rcx
	call ExitProcess
;;	/return
;;	/elif
label3:
;;	else
;;	return
	mov rdx, 5
	push rdx
	pop rcx
	call ExitProcess
;;	/return
;;	/else
	label1:
;;	return
	mov rdx, 10
	push rdx
	pop rcx
	call ExitProcess
;;	/return
