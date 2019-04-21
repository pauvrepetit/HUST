.386
data segment use16
	bname	DB	'huao', 6 dup(0), 4		;boss name
	bpass	DB	'huao', 6 dup(0), 4		;password
	N		EQU	30						;number
	shopname	DB	'huao$'				;shop name

	auth	DB	?

	ga1		DB	'pen$', 6 dup(0)
			DB	3
			DB	10
			DW	35, 56, 70, 25, ?

	ga2		DB	'book$', 5 dup(0)
			DB	4
			DB	9
			DW	12, 30, 25, 5, ?

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

	call findgood
	cmp ch, -1
	jz printNotFound

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

	jmp start

authNotOK:
	mov al, [bx + 12]
	mov cl, 128
	mul cl
	push ax
	mov al, [bx + 14]
	mov cl, [bx + 11]
	mul cl
	mov cl, 10
	div cl
	mov ah, 0
	mov dx, ax
	pop ax
	div dl
	mov ah, 0
	jmp func4

printNotFound:
	lea dx, notFound[0]
	mov ah, 9
	int 21h
	jmp recommandGood

func4:
	cmp ax, 100
	jns printA
	cmp ax, 50
	jns printB
	cmp ax, 10
	jns printC
	jmp printF

printA:
	mov dl, 41H
	mov ah, 2
	int 21h
	jmp returnStart

printB:
	mov dl, 42H
	mov ah, 2
	int 21h
	jmp returnStart

printC:
	mov dl, 43H
	mov ah, 2
	int 21h
	jmp returnStart

printF:
	mov dl, 46H
	mov ah, 2
	int 21h
	jmp returnStart

returnStart:
	lea dx, outenter[0]
	mov ah, 9
	int 21h
	jmp start

Quit:
	mov ah, 4Ch
	int 21h

findgood proc
	mov bl, goodname[1]
	mov bh, 0
	mov si, bx

	lea bx, ga1[0]
	mov al, [bx + 10]
	mov ah, 0
	cmp ax, si
	jnz next1

	call compare
	cmp ch, 1
	jz foundgood

next1:
	lea bx, ga2[0]
	mov al, [bx + 10]
	mov ah, 0
	cmp ax, si
	jnz next2

	call compare
	cmp ch, 1
	jz foundgood

next2:
	mov ch, -1
	ret

foundgood:
	mov ch, bh
	ret
findgood endp

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

code ends
	end start
