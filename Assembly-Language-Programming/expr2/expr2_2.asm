.386

goodStruct struct
	good_name	DB	10 dup(0)
	good_len	DB	0
	good_sale	DB	10
	good_in_price	DW	?
	good_out_price	DW	?
	good_all_count	DW	?
	good_sale_count	DW	?
	good_recommamd	DW	?
goodStruct ends

data segment use16
	bname	DB	'huao', 6 dup(0), 4		;boss name
	bpass	DB	'huao', 6 dup(0), 4		;password
	N		EQU	1000					;number
	shopname	DB	'huao$'				;shop name

	auth	DB	?
	numOfBag	EQU		1000

	ga1		goodStruct	<'pen$', 3, 10, 35, 56, 70, 25, ?>
	ga2		goodStruct	<'book$', 4, 9, 12, 30, 25, 5, ?>

	goodStruct	N-3	dup(<'egg$', 3, 7, 35, 56, 70, 25, ?>)

	bag		goodStruct	<'bag$', 3, 10, 20, 50, numOfBag, 0, ?>

	goodLen	EQU	$ - bag

	welcomeMsg	DB	'welcome to ', '$'
	welcomeShopMsg	DB	' shop$'

	loginErrorMsg	DB	'Login Error', 0ah, 0dh, '$'

	outenter	DB	0ah, 0dh, '$'
	info	DB	'input your name and password', 0ah, 0dh, '$'
	in_name	DB	10, ?, 10 dup(0), '$'
	in_pass	DB	6, ?, 6 dup(0), '$'

	checkname	DB	'input commodity you want to check:', '$'
	goodname	DB	10, ?, 10 dup(0)
	notFound	DB	'good not found', 0ah, 0dh, '$'
data ends

stack segment use16 stack
	DB 200 dup(0)
stack ends

code segment use16
	assume DS:data, CS:code, SS:stack
start:
	mov ax, data
	mov ds, ax

	lea dx, welcomeMsg[0]
	mov ah, 9
	int 21h

	lea	dx, shopname[0]
	mov ah, 9
	int 21h

	lea dx, welcomeShopMsg[0]
	mov ah, 9
	int 21h

	lea dx, outenter[0]
	mov ah, 9
	int 21h

	lea dx, info[0]
	mov ah, 9
	int 21h

	lea dx, in_name[0]
	mov ah, 10
	int 21h

	lea dx, outenter[0]
	mov ah, 9
	int 21h

	mov al, in_name[1]
	cbw
	mov bx, ax
	mov ax, 0
	mov in_name[bx + 2], al

	mov al, in_name[1]
	cmp al, 0
	jz user
	cmp al, 1
	jz checkQuit
	jmp next3

checkQuit:
	mov al, in_name[2]
	cmp al, 71h
	jz quit

next3:
	lea dx, in_pass[0]
	mov ah, 10
	int 21h

	mov al, in_pass[1]
	cbw
	mov bx, ax
	mov ax, '$'
	mov in_pass[bx + 2], al

	lea dx, outenter[0]
	mov ah, 9
	int 21h

	lea bx, in_name[2]
	mov al, in_name[1]
	cmp al, bname[10]
	jnz errorLogin
	call compareName
	cmp ch, 1
	jnz errorLogin

	lea bx, in_pass[2]
	mov al, in_pass[1]
	cmp al, bpass[10]
	jnz errorLogin
	call comparePass
	cmp ch, 1
	jnz errorLogin

	mov al, 1
	mov auth[0], al
	jmp recommandGood

errorLogin:
	lea dx, loginErrorMsg[0]
	mov ah, 9
	int 21h
	jmp start

user:
	mov al, 0
	mov auth[0], al
	jmp recommandGood

recommandGood:
	lea dx, checkname[0]
	mov ah, 9
	int 21h

	lea dx, goodname[0]
	mov ah, 10
	int 21h

	lea dx, outenter[0]
	mov ah, 9
	int 21h

	mov cl, goodname[1]
	cmp cl, 0
	jz start

	mov ax, 0
	call TIMER


	call findgood
	cmp ch, -1
	jz printNotFound

	mov ax, [bx].good_all_count

	mov cl, auth[0]
	cmp cl, 1
	jz authOK
	jmp authNotOK

authOK:
	mov dx, bx
	mov ah, 9
	int 21h

	lea dx, outenter[0]
	mov ah, 9
	int 21h

	mov ax, 1
	call TIMER

	lea dx, outenter[0]
	mov ah, 9
	int 21h

	jmp start

authNotOK:
	call calIndex
	cmp ax, numOfBag
	jnz func4
buygood:
	mov cx, [bx].good_sale_count
	inc cx
	mov [bx].good_sale_count, cx
	call calIndex
	dec ax
	; cmp ax, 0
	jnz buygood
	
	jmp func4

func4:
	mov ax, 1
	call TIMER

	mov dl, [bx + 20]
	mov ah, 2
	int 21H
	jmp returnStart

returnStart:
	lea dx, outenter[0]
	mov ah, 9
	int 21h

	jmp start

printNotFound:
	mov ax, 1
	call TIMER
	lea dx, notFound[0]
	mov ah, 9
	int 21h
	jmp recommandGood

Quit:
	mov ah, 4Ch
	int 21h


findgood proc
	mov bl, goodname[1]
	mov bh, 0
	mov si, bx

	lea bx, ga1[0]
	sub bx, goodLen

	mov dx, N
	inc dx

goodLoop:
	dec dx
	add bx, goodLen
	cmp dx, 0
	jz next2
	mov al, [bx + 10]
	mov ah, 0
	cmp ax, si
	jnz goodLoop
	call compare
	cmp ch, 1
	jz foundgood
	jmp goodLoop

next2:
	mov ch, -1
	ret

foundgood:
	mov ch, bh
	ret
findgood endp

calIndex proc
	pusha
	
	lea bx, ga1[0]
	sub bx, goodLen

	mov di, N+1

calLoop:
	add bx, goodLen
	dec di
	jz calEnd

	mov ax, [bx].good_in_price
	mov cx, 1280
	mul cx
	mov bp, dx
	mov si, ax
	mov al, byte ptr [bx].good_out_price
	mov cl, [bx].good_sale
	mul cl
	mov cx, ax
	mov dx, bp
	mov ax, si
	div cx

	mov bp, ax
	mov ax, [bx].good_sale_count
	mov cx, 64
	mul cx
	mov cx, [bx].good_all_count
	div cx
	add ax, bp

	cmp ax, 100
	jns recommA
	cmp ax, 50
	jns recommB
	cmp ax, 10
	jns recommC
	jmp recommF

recommA:
	mov bp, 'A'
	mov [bx + 20], bp
	jmp calLoop
recommB:
	mov bp, 'B'
	mov [bx + 20], bp
	jmp calLoop
recommC:
	mov bp, 'C'
	mov [bx + 20], bp
	jmp calLoop
recommF:
	mov bp, 'F'
	mov [bx + 20], bp
	jmp calLoop

calEnd:
	popa
	ret
calIndex endp


compare proc
comp:
	cmp al, 0
	jz found
	dec al
	mov ah, 0
	mov di, ax
	mov dl, goodname[2 + di]
	sub dl, [bx + di]
	jz comp

	mov ch, 0
	ret
found:
	mov ch, 1
	ret
compare endp

compareName proc
compName:
	cmp al, 0
	jz trueName
	dec al
	mov ah, 0
	mov di, ax
	mov dl, bname[di]
	sub dl, [bx + di]
	jz compName

	mov ch, 0
	ret
trueName:
	mov ch, 1
	ret
compareName endp

comparePass proc
compPass:
	cmp al, 0
	jz truePass
	dec al
	mov ah, 0
	mov di, ax
	mov dl, bpass[di]
	sub dl, [bx + di]
	jz compPass

	mov ch, 0
	ret
truePass:
	mov ch, 1
	ret
comparePass endp

TIMER	PROC
	PUSH  DX
	PUSH  CX
	PUSH  BX
	MOV   BX, AX
	MOV   AH, 2CH
	INT   21H	     ;CH=hour(0-23),CL=minute(0-59),DH=second(0-59),DL=centisecond(0-100)
	MOV   AL, DH
	MOV   AH, 0
	IMUL  AX,AX,1000
	MOV   DH, 0
	IMUL  DX,DX,10
	ADD   AX, DX
	CMP   BX, 0
	JNZ   _T1
	MOV   CS:_TS, AX
_T0:	POP   BX
	POP   CX
	POP   DX
	RET
_T1:	SUB   AX, CS:_TS
	JNC   _T2
	ADD   AX, 60000
_T2:	MOV   CX, 0
	MOV   BX, 10
_T3:	MOV   DX, 0
	DIV   BX
	PUSH  DX
	INC   CX
	CMP   AX, 0
	JNZ   _T3
	MOV   BX, 0
_T4:	POP   AX
	ADD   AL, '0'
	MOV   CS:_TMSG[BX], AL
	INC   BX
	LOOP  _T4
	PUSH  DS
	MOV   CS:_TMSG[BX+0], 0AH
	MOV   CS:_TMSG[BX+1], 0DH
	MOV   CS:_TMSG[BX+2], '$'
	LEA   DX, _TS+2
	PUSH  CS
	POP   DS
	MOV   AH, 9
	INT   21H
	POP   DS
	JMP   _T0
_TS	DW    ?
 	DB    'Time elapsed in ms is '
_TMSG	DB    12 DUP(0)
TIMER   ENDP

code ends
	end start
