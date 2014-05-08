[section .data]

setcolor db 1Bh, '[49;34m', 0 
    .len equ $ - setcolor
basecolor db 1Bh, '[0;0m', 0 
    .len equ $ - basecolor

[section .text]
global _my_print
global _change_color
global _ret_color
extern _printf
_syscall:             
    int     0x80            ;system call  
    ret  
_my_print:
    push dword 1
    push dword [esp+8]
    push dword 1
    mov eax, 0x4
    call _syscall
    add esp, 12
    ret

_change_color:
	push dword setcolor.len
    push dword setcolor
    push dword 1
    mov eax, 0x4
    call _syscall
    add esp, 12
    ret

_ret_color:
	push dword basecolor.len
    push dword basecolor
    push dword 1
    mov eax, 0x4
    call _syscall
    add esp, 12
    ret
