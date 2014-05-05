[section .text]
global my_print
my_print:
	mov	edx,[esp+8]
	mov	ecx,[esp+4]
	mov	ebx,1
	mov	eax,4
	int	0x80
	ret
	
	
