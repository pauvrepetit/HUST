.386
STACK SEGMENT USE16 STACK
        DB 200 DUP(0)
STACK ENDS

DATA SEGMENT USE16
BUF1    DB 0,1,2,3,4,5,6,7,8,9
BUF2    DB 10 DUP(0)
BUF3    DB 10 DUP(0)
BUF4    DB 10 DUP(0)
DATA ENDS

CODE SEGMENT USE16
    ASSUME CS:CODE, DS:DATA, SS:STACK
START:  
        MOV AX,DATA
        MOV DS,AX
        MOV ESI,0
        MOV EDI,0
        MOV EBX,0
        MOV EBP,0
        MOV CX,10
LOPA:
        MOV AL, BUF1[ESI]
        MOV BUF2[EDI],AL
        INC AL
        MOV BUF3[EBX], AL
        ADD AL,3
        MOV BUF4[EBP],AL
        INC ESI
        INC EDI
        INC EBP
        INC EBX
        DEC CX
        JNZ LOPA
        MOV AH,4CH
        INT 21H
CODE ENDS
        END START
