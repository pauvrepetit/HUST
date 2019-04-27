.386

code segment use16
start:
    mov ax, 0
    mov es, ax
    mov dword ptr es:[16H * 4], 0F00011E0H
    mov ah, 4Ch
    int 21h
code ends
    end start