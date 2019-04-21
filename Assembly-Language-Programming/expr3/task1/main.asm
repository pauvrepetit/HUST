extrn outone:far, sort:far, outall:far, change_good:far

public io_1, io_2, io_3, io_4, io_5, io_6, io_7, io_8, BUF1, bufF2T10, DAT, SIGN
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
	good_sort		DW	1
goodStruct ends

data segment use16
    bname	DB	'huao', 6 dup(0), 4		;boss name
	bpass	DB	'huao', 6 dup(0), 4		;password
	N		EQU	10  					;number
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


    menu1       DB  '1. Query product information', 0ah, 0dh, '$'
    menu2       DB  '2. Edit product information', 0ah, 0dh, '$'
    menu3       DB  '3. Calculate recommendation', 0ah, 0dh, '$'
    menu4       DB  '4. Calculate recommendation ranking', 0ah, 0dh, '$'
    menu5       DB  '5. Output all product information', 0ah, 0dh, '$'
    menu6       DB  '6. exit the program', 0ah, 0dh, '$'

    errorInputMsg DB 'Input Error. Input again.', 0ah, 0dh, '$'

    io_1 db 'good_name is: $'
	io_2 db 'good_sale is: $'
	io_3 db 'good_in_price is: $'
	io_4 db 'good_out_price is: $'
	io_5 db 'good_all_count is: $'
	io_6 db 'good_sale_count is: $'
	io_7 db 'good_recommand is: $'
	io_8 db 'good_sort is: $'

    BUF1 db 10, ?, 10 dup(0)
	bufF2T10 db 12 dup(0)
	DAT DW 0AH
	SIGN DB ?
data ends

stack segment use16 stack
    DB 200 dup(0)
stack ends

code segment use16


WelcomeFunc PROTO near stdcall
InputString PROTO near stdcall stringInputAddr:WORD
ShowMenu PROTO near stdcall userType:BYTE
ShowString PROTO near stdcall stringAddr:WORD
ExitProgram PROTO near stdcall
quaryGood proto near stdcall

start:
    mov ax, data
    mov ds, ax
    invoke WelcomeFunc
    mov bx, offset auth

chooseFunc:
    invoke ShowMenu, ds:[bx]

    mov ah, 1
    int 21h
    mov bx, offset outenter
    invoke ShowString, bx
    ;此时al中为功能数
    cmp al, '1'
    jz func1
    cmp al, '2'
    jz func2
    cmp al, '3'
    jz func3
    cmp al, '4'
    jz func4
    cmp al, '5'
    jz func5
    cmp al, '6'
    jz func6
    
    mov bx, offset errorInputMsg
    invoke ShowString, bx
    jmp chooseFunc

func1:
	invoke quaryGood
	jmp start

func2:
	push di

	mov bx, offset checkname
    invoke ShowString, bx

    mov bx, offset goodname
    invoke InputString, bx

    mov bx, offset outenter
    invoke ShowString, bx

    mov si, offset goodname
	mov cl, [si + 1]
	cmp cl, 0
	jz start

    call findgood
	cmp ch, -1
	jz printNotFound2

	mov di, bx
	call change_good
	pop di
	jmp start

printNotFound2:
    mov dx, offset notFound
    invoke ShowString, dx
	jmp func2

func3:
    call calIndex
    jmp start
func4:
    mov ax, N
    mov si, offset ga1
    call sort
    jmp start
func5:
    mov ax, N
    mov si, offset ga1
    call outall
    jmp start
func6:
    invoke ExitProgram


quaryGood proc near stdcall
quaryGoodBegin:
    mov bx, offset checkname
    invoke ShowString, bx

    mov bx, offset goodname
    invoke InputString, bx

    mov bx, offset outenter
    invoke ShowString, bx

    mov si, offset goodname
	mov cl, [si + 1]
	cmp cl, 0
	jz EndJmpStart

    call findgood
	cmp ch, -1
	jz printNotFound

    mov si, offset auth
    mov cl, [si]
	cmp cl, 1
	jz authOK
	jmp authNotOK

authOK:
    push si
    mov si, bx
    call outone
    pop si
	jmp EndJmpStart

authNotOK:
    push si
    mov si, bx
    call outone
    pop si
    jmp EndJmpStart

printNotFound:
    mov dx, offset notFound
    invoke ShowString, dx
	jmp quaryGoodBegin
EndJmpStart:
	ret
quaryGood endp

WelcomeFunc PROC near stdcall uses ax bx dx
welcomeStart:
    mov ax, offset welcomeMsg
    invoke ShowString, ax
    mov ax, offset shopname
    invoke ShowString, ax
    mov ax, offset welcomeShopMsg
    invoke ShowString, ax
    mov ax, offset outenter
    invoke ShowString, ax
    mov ax, offset info
    invoke ShowString, ax
    mov ax, offset in_name
    invoke InputString, ax
    mov ax, offset outenter
    invoke ShowString, ax
;下面将读入的回车符号置为0
    mov si, offset in_name
    mov al, ds:[si + 1]
    mov ah, 0
    mov bx, ax
    mov ax, 0
    mov ds:[si + bx + 2], al

    mov al, ds:[si + 1]
    cmp al, 0
    jz welcomeUser
    cmp al, 1
	jz checkQuit
	jmp next3

checkQuit:
	mov al, ds:[si + 2]
	cmp al, 71h
	invoke ExitProgram

next3:
	mov dx, offset in_pass
    invoke InputString, dx

    mov si, offset in_pass
	mov al, ds:[si + 1]
	mov ah, 0
	mov bx, ax
	mov ax, 0
	mov ds:[si + bx + 2], al

	mov dx, offset outenter
    invoke ShowString, dx

    lea bx, in_name[2]
    mov si, offset in_name
	mov al, ds:[si + 1]
    mov si, offset bname
	cmp al, ds:[si + 10]
	jnz errorLogin
	call compareName
	cmp ch, 1
	jnz errorLogin

	lea bx, in_pass[2]
    mov si, offset in_pass
	mov al, ds:[si + 1]
    mov si, offset bpass
	cmp al, ds:[si + 10]
	jnz errorLogin
	call comparePass
	cmp ch, 1
	jnz errorLogin

	mov al, 1
    mov si, offset auth
	mov [si], al
    ret

errorLogin:
	lea dx, loginErrorMsg[0]
	mov ah, 9
	int 21h
	jmp welcomeStart

welcomeUser:
    mov al, 0
    mov si, offset auth
	mov [si], al
    ret
WelcomeFunc ENDP


InputString PROC near stdcall uses dx ax, stringInputAddr:WORD
    mov dx, stringInputAddr
    mov ah, 10
    int 21h
    ret
InputString ENDP


ShowMenu PROC near stdcall uses ax, userType:BYTE
    mov ax, offset menu1
    invoke ShowString, ax
    cmp userType, 1
    jnz user
    mov ax, offset menu2
    invoke ShowString, ax
    mov ax, offset menu3
    invoke ShowString, ax
    mov ax, offset menu4
    invoke ShowString, ax
    mov ax, offset menu5
    invoke ShowString, ax
user:
    mov ax, offset menu6
    invoke ShowString, ax
    ret
ShowMenu ENDP

ShowString PROC near stdcall uses dx ax, stringAddr:WORD
    mov dx, stringAddr
    mov ah, 9
    int 21h
    ret
ShowString ENDP

ExitProgram PROC near stdcall
    mov ax, 4C00h
    int 21h
ExitProgram ENDP

compareName proc
    push si
compName:
	cmp al, 0
	jz trueName
	dec al
	mov ah, 0
	mov di, ax
    mov si, offset bname
    add si, di
	mov dl, [si]
	sub dl, [bx + di]
	jz compName

	mov ch, 0
    pop si
	ret
trueName:
	mov ch, 1
    pop si
	ret
compareName endp

comparePass proc
    push si
compPass:
	cmp al, 0
	jz truePass
	dec al
	mov ah, 0
	mov di, ax
    mov si, offset bpass
    add si, di
	mov dl, [si]
	sub dl, [bx + di]
	jz compPass

	mov ch, 0
    pop si
	ret
truePass:
	mov ch, 1
    pop si
	ret
comparePass endp

findgood proc
    push di
    mov di, offset goodname
	mov bl, [di + 1]
	mov bh, 0
	mov si, bx

    mov di, offset ga1
	lea bx, [di]
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
    pop di
	ret

foundgood:
	mov ch, bh
    pop di
	ret
findgood endp

compare proc
    push si
comp:
	cmp al, 0
	jz found
	dec al
	mov ah, 0
	mov di, ax
    mov si, offset goodname
    add si, di
	mov dl, [si + 2]
	sub dl, [bx + di]
	jz comp

	mov ch, 0
    pop si
	ret
found:
	mov ch, 1
    pop si
	ret
compare endp

calIndex proc
	pusha
	
    mov si, offset ga1
	lea bx, [si]
	sub bx, goodLen

	mov di, N
	inc di

calLoop:
	dec di
	add bx, goodLen
	cmp di, 0
	jz calEnd

	mov ax, [bx].good_in_price
	mov cx, 1280
	mul cx
	push dx
	push ax
	mov al, byte ptr [bx].good_out_price
	mov cl, [bx].good_sale
	mul cl
	mov cx, ax
	pop ax
	pop dx
	div cx
	push ax

	mov ax, [bx].good_sale_count
	mov cx, 64
	mul cx
	mov cx, [bx].good_all_count
	div cx
	mov dx, ax
	pop ax
	add ax, dx

	cmp ax, 100
	jns recommA
	cmp ax, 50
	jns recommB
	cmp ax, 10
	jns recommC
	jmp recommF

recommA:
	push ax
	mov ax, 'A'
	mov [bx + 20], al
	pop ax
	jmp calLoop
recommB:
	push ax
	mov ax, 'B'
	mov [bx + 20], al
	pop ax
	jmp calLoop
recommC:
	push ax
	mov ax, 'C'
	mov [bx + 20], al
	pop ax
	jmp calLoop
recommF:
	push ax
	mov ax, 'F'
	mov [bx + 20], al
	pop ax
	jmp calLoop

calEnd:
	popa
	ret
calIndex endp


code ends
    end start