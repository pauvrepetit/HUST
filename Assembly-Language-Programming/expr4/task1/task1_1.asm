.386
code segment use16
start:
    mov ah, 35h
    mov al, 1h
    int 21h
    mov ah, 35h
    mov al, 13h
    int 21h
code ends
stack segment use16 stack
    DB 200 dup(0)
stack ends
    end start
