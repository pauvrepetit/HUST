.386
.model flat, c
option casemap:none

public WelcomeFunc, ShowMenu, calIndex, ExitProgram
;public io_1, io_2, io_3, io_4, io_5, io_6, io_7, io_8, BUF1, bufF2T10, DAT, SIGN
INCLUDELIB USER32.LIB
INCLUDELIB KERNEL32.LIB

goodStruct struct
	good_name	DB	10 dup(0)
	good_len	DB	0
	good_sale	DB	10
	good_in_price	DW	?
	good_out_price	DW	?
	good_all_count	DW	?
	good_sale_count	DW	?
	good_recommand	DW	?
	good_sort		DW	1
goodStruct ends

.data
	scanfArgStr DB '%s', 0

.code code2
printf proto cdecl :dword
scanf proto cdecl :dword, :dword
strlen proto cdecl :dword
InputString proto stdcall stringInputAddr:dword
ShowString proto stdcall stringAddr:dword
ExitProgram proto cdecl
compareName proto stdcall :dword, :dword
comparePass proto stdcall :dword, :dword
;findgood proto stdcall
;compare proto stdcall

ExitProcess proto stdcall :dword
MessageBoxA proto stdcall :dword,:dword, :dword,:dword

WelcomeFunc PROC uses eax ebx edx, welcomeMsg:dword, shopname:dword, welcomeShopMsg:dword, outenter:dword, info:dword, in_name:dword, in_pass:dword, bname:dword, bpass:dword, auth:dword, loginErrorMsg:dword
welcomeStart:
    mov eax, welcomeMsg
	invoke ShowString, eax
    mov eax, shopname
    invoke ShowString, eax
    mov eax, welcomeShopMsg
    invoke ShowString, eax
    mov eax, outenter
    invoke ShowString, eax
    mov eax, info
    invoke ShowString, eax
    mov eax, in_name
    invoke InputString, eax
;下面将读入的回车符号置为0
    mov esi, in_name
    mov al, ds:[esi + 1]
    mov ah, 0
    movsx ebx, ax
    mov ax, 0
    mov ds:[esi + ebx + 2], al

    mov al, ds:[esi + 1]
    cmp al, 0
    jz welcomeUser
    cmp al, 1
	jz checkQuit
	jmp next3

checkQuit:
	mov al, [esi + 2]
	cmp al, 71h
	jnz errorLogin
	invoke ExitProgram

next3:
	mov edx, in_pass
    invoke InputString, edx

    mov esi, in_pass
	mov al, ds:[esi + 1]
	mov ah, 0
	movsx ebx, ax
	mov ax, 0
	mov ds:[esi + ebx + 2], al

    lea ebx, in_name[2]
    mov esi, in_name
	mov al, ds:[esi + 1]
    mov esi, bname
	cmp al, ds:[esi + 10]
	jnz errorLogin
	invoke compareName, bname, in_name
	cmp ch, 1
	jnz errorLogin

	lea ebx, in_pass[2]
    mov esi, in_pass
	mov al, ds:[esi + 1]
    mov esi, bpass
	cmp al, ds:[esi + 10]
	jnz errorLogin
	invoke comparePass, bpass, in_pass
	cmp ch, 1
	jnz errorLogin

	mov al, 1
    mov esi, auth
	mov [esi], al
    ret

errorLogin:
	invoke ShowString, loginErrorMsg
	jmp welcomeStart

welcomeUser:
    mov al, 0
    mov esi, auth
	mov [esi], al
    ret
WelcomeFunc ENDP


InputString PROC stdcall uses edx eax, stringInputAddr:DWORD
	mov eax, stringInputAddr
	lea edx, scanfArgStr
	add eax, 2
	invoke scanf, edx, eax
	mov dl, 10
	mov eax, stringInputAddr
	mov [eax], dl
	add eax, 2
	invoke strlen, eax
	mov edx, stringInputAddr
	mov [edx + 1], al
	mov [edx + 12], 0
    ret
InputString ENDP

ShowMenu PROC uses eax, userType:BYTE, menu1:dword, menu2:dword, menu3:dword, menu4:dword, menu5:dword, menu6:dword
    mov eax, menu1
    invoke ShowString, eax
    cmp userType, 1
    jnz user
    mov eax, menu2
    invoke ShowString, eax
    mov eax, menu3
    invoke ShowString, eax
    mov eax, menu4
    invoke ShowString, eax
    mov eax, menu5
    invoke ShowString, eax
user:
    mov eax, menu6
    invoke ShowString, eax
    ret
ShowMenu ENDP

ShowString PROC stdcall stringAddr:DWORD
	invoke printf, stringAddr
	ret
ShowString ENDP

ExitProgram PROC
	invoke ExitProcess, 0
	ret
ExitProgram ENDP

compareName proc stdcall, bname:dword, in_name:dword
	mov ebx, in_name
	add ebx, 2
    push esi
compName:
	cmp al, 0
	jz trueName
	dec al
	mov ah, 0
	mov di, ax
	movsx edi, di
    mov esi, bname
    add esi, edi
	mov dl, [esi]
	sub dl, [ebx + edi]
	jz compName

	mov ch, 0
    pop esi
	ret
trueName:
	mov ch, 1
    pop esi
	ret
compareName endp

comparePass proc stdcall, bpass:dword, in_pass:dword
	mov ebx, in_pass
	add ebx, 2
    push esi
compPass:
	cmp al, 0
	jz truePass
	dec al
	mov ah, 0
	movsx edi, ax
    mov esi, bpass
    add esi, edi
	mov dl, [esi]
	sub dl, [ebx + edi]
	jz compPass

	mov ch, 0
    pop esi
	ret
truePass:
	mov ch, 1
    pop esi
	ret
comparePass endp

findgood proc stdcall, goodname:dword, ga1:dword, goodLen:word, N:word
    push edi
    mov edi, goodname
	mov bl, [edi + 1]
	mov bh, 0
	mov si, bx

    mov edi, ga1
	lea bx, [edi]
	sub bx, goodLen

	mov dx, N
	inc dx

goodLoop:
	dec dx
	add bx, goodLen
	cmp dx, 0
	jz next2
	movsx ebx, bx
	mov al, [ebx + 10]
	mov ah, 0
	cmp ax, si
	jnz goodLoop
	call compare
	cmp ch, 1
	jz foundgood
	jmp goodLoop

next2:
	mov ch, -1
    pop edi
	ret

foundgood:
	mov ch, bh
    pop edi
	ret
findgood endp

compare proc stdcall, goodname:dword
    push esi
	movsx ebx, bx
comp:
	cmp al, 0
	jz found
	dec al
	mov ah, 0
	movsx edi, ax
    mov esi, goodname
    add esi, edi
	mov dl, [esi + 2]
	sub dl, [ebx + edi]
	jz comp

	mov ch, 0
    pop esi
	ret
found:
	mov ch, 1
    pop esi
	ret
compare endp

calIndex proc uses eax ebx ecx edx esi edi ga1:dword, goodLen:dword, N:word	
    mov esi, ga1
	lea ebx, [esi]
	sub ebx, goodLen

	mov di, N
	inc di

calLoop:
	dec di
	add ebx, goodLen
	cmp di, 0
	jz calEnd

	mov ax, [ebx].good_in_price
	mov cx, 1280
	mul cx
	push dx
	push ax
	mov al, byte ptr [ebx].good_out_price
	mov cl, [ebx].good_sale
	mul cl
	mov cx, ax
	pop ax
	pop dx
	div cx
	push ax

	mov ax, [ebx].good_sale_count
	mov cx, 64
	mul cx
	mov cx, [ebx].good_all_count
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
	mov [ebx + 20], al
	pop ax
	jmp calLoop
recommB:
	push ax
	mov ax, 'B'
	mov [ebx + 20], al
	pop ax
	jmp calLoop
recommC:
	push ax
	mov ax, 'C'
	mov [ebx + 20], al
	pop ax
	jmp calLoop
recommF:
	push ax
	mov ax, 'F'
	mov [ebx + 20], al
	pop ax
	jmp calLoop

calEnd:
	ret
calIndex endp

end