.386
data segment use16
    CMOSaddr db 2
data ends

stack segment use16 stack
    db 200 dup(0)
stack ends

code segment use16
    assume ds:data, cs:code, ss:stack
start:
    mov ax, data
    mov ds, ax
    mov al, 0
    mov al, CMOSaddr
    out 70h, al
    in al, 71h
    mov bl, al

    mov ah, 2
    mov dl, bl
    shr dl, 4
    add dl, '0'
    int 21h

    mov ah, 2
    mov dl, bl
    shl dl, 4
    shr dl, 4
    add dl, '0'
    int 21h

    mov ax, 4C00H
    int 21h

code ends
    end start