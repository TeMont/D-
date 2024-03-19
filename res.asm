global _start
start:
	mov rax, 60
	mov rdi, 69
	syscall
	mov rax, 60
	mov rdi, 10
	syscall
