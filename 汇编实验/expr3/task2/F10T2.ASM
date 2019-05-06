	;有符号十进制转换为字类型的二进制数F10T2
               ;入口参数：输入串首址在SI中，串长在CX中
               ;出口参数：(SI)=-1表示出错，转换结果在AX中。
.386
.model flat, stdcall
option casemap:none

PUBLIC F10T2
;extrn SIGN:BYTE, DAT:WORD

;F10T2 proto stdcall :dword, :word

;CODE SEGMENT PARA PUBLIC 'CODE'
;	ASSUME CS:CODE
.code
F10T2 PROC uses esi ecx di strAddr:dword, strLen:word, SIGN:dword, DAT:dword
	PUSH DX
	PUSH BX
	mov esi, strAddr
	mov cx, strLen
	MOV AX,0
	MOV byte ptr [SIGN],0
	MOV BL,[ESI]
	CMP BL,'+'
	JZ  A
	CMP BL,'-'
	JNE NEXT2
	MOV byte ptr [SIGN], 1
A:
	DEC CX
	JZ ERR
NEXT1:
	INC ESI
	MOV BL,[ESI]
NEXT2:
	CMP BL,'0'
	JB ERR
	CMP BL,'9'
	JA ERR
	SUB BL,30H
	MOV BH,0
	;mov di, word ptr [DAT]
	MOV di, 10
	MUL di
	JO ERR
	ADD AX,BX
	JC ERR
	
	DEC CX
	JNZ NEXT1
	CMP byte ptr [SIGN], 1
	JNE QQ
	NEG AX

QQ:
	POP BX
	POP DX
	RET
ERR:
	MOV ESI,-1
	JMP QQ
F10T2	ENDP
;CODE ENDS
END
