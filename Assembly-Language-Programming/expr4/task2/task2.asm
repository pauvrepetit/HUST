.386
code segment use16
    assume cs:code, ss:stack
oldINT16H dw ?,?
newINT16H:
    cmp ah, 0
    jz trans
    cmp ah, 10h
    jz trans
    jmp dword ptr oldINT16H
trans:
    pushf
    call dword ptr oldINT16H
    cmp al, 'A'
    jb nottrans
    cmp al, 'Z'
    ja nottrans
    add al, 20h
nottrans:
    iret

start:
    mov ax, 0
    mov es, ax
    mov ax, es:[16H * 4]
    mov oldINT16H, ax
    mov ax, es:[16H * 4 + 2]
    mov oldINT16H + 2, ax

    CLI
    mov word ptr es:[16H * 4], offset newINT16H
    mov es:[16H * 4 + 2], cs
    STI

    mov dx, offset start + 15
    shr dx, 4
    add dx, 10h
    mov ah, 31h
    int 21h
code ends
stack segment use16 stack
    db 200 dup(0)
stack ends
    end start