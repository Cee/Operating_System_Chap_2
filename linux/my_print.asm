[section .data]

setcolor db 1Bh, '[49;34m', 0 
    .len equ $ - setcolor
basecolor db 1Bh, '[0;0m', 0 
    .len equ $ - basecolor

[section .text]
global _my_print
global _change_color
global _ret_color
_my_print:
	mov	edx,[esp+8]
	mov	ecx,[esp+4]
	mov	ebx,1
	mov	eax,4 
	int	0x80
	ret

_change_color:
    mov eax, 4
    mov ebx, 1
    mov ecx, setcolor
    mov edx, setcolor.len
    int 80h
    ret

_ret_color:
	mov eax, 4
    mov ebx, 1
    mov ecx, basecolor
    mov edx, basecolor.len
    int 80h
    ret