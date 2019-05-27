.386

putnum MACRO
    local large, endputnum
    mov ah, 2
    cmp dl, 9
    ja large
    add dl, '0'
    jmp endputnum
large:
    add dl, 'A'
    sub dl, 10
endputnum:
    int 21h
ENDM

data segment use16
    CMOSaddr db 2        ; 修改此处的值以指定待读取的CMOS单元地址
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

    mov dl, bl
    shr dl, 4
    putnum

    mov dl, bl
    shl dl, 4
    shr dl, 4
    putnum

    mov ax, 4C00H
    int 21h

code ends
    end start