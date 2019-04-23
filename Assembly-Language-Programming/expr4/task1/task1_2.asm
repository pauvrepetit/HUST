.386
stack segment use16 stack
    db 200 dup(0)
stack ends
code segment use16
start:
    mov ax, 0
    mov ds, ax
    
    mov es, ds:[6]
    mov bx, ds:[4]

    mov es, ds:[4EH]
    mov bx, ds:[4CH]

code ends
    end start