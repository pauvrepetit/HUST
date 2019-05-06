.386
.model flat, c
option casemap:none

public outone, sort, outall, change_good

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

printf proto cdecl :dword
putchar proto cdecl :byte
InputString proto stdcall :dword
;output string$
puts macro ofset:req
	push eax
	mov eax, offset ofset
	invoke printf, eax
	pop eax
endm

;input change cx si,cx is length, si is addr 
gets macro ofset:req
	lea esi, ofset
	invoke InputString, esi
	mov cl, [esi + 1]
	movsx cx, cl
	add esi, 2
endm

CRLF macro
	invoke putchar, 0ah
	invoke putchar, 0dh
endm

outone proto :dword
F10T2 proto stdcall :dword, :word, :dword, :dword

.data
	io_1 db 'good_name is: ', 0
	io_2 db 'good_sale is: ', 0
	io_3 db 'good_in_price is: ', 0
	io_4 db 'good_out_price is: ', 0
	io_5 db 'good_all_count is: ', 0
	io_6 db 'good_sale_count is: ', 0
	io_7 db 'good_recommand is: ', 0
	io_8 db 'good_sort is: ', 0
	BUF1 db 10, ?, 10 dup(0)
	bufF2T10 db 12 dup(0)

.stack 200

.code code1
	;assume cs:code1
;ax good sum, si good addr
sort proc goodSum:word, goodAddr:dword
	push ebx
	push cx
	push dx
	mov ax, goodSum
	mov esi, goodAddr
	jmp L3
L4:	
	dec ax
	add esi, sizeof goodStruct
	cmp ax, 1
	je exit
L3:
	mov dx, [esi].good_recommand
	mov ebx, esi
	mov cx, ax
L5:	
	add ebx, sizeof goodStruct
	dec cx
	cmp dx, [ebx].good_recommand
	ja	L1 
	jb	L2
next:   
	cmp cx, 1
	je L4
	jmp L5
L1:	
	inc [ebx].good_sort
	jmp next
L2:
	inc [esi].good_sort
	jmp next
exit:
	pop dx
	pop cx
	pop ebx
	ret
sort endp

; si good addr
outone proc uses eax esi edx goodAddr:dword
	mov esi, goodAddr
	mov dx, 16
L6:
	puts io_1
	invoke printf, esi
	CRLF
	puts io_2
	xor ax, ax
	mov al, [esi].good_sale
	call F2T10
	CRLF
	puts io_3
	mov ax, [esi].good_in_price
	call F2T10
	CRLF
	puts io_4
	mov ax, [esi].good_out_price
	call F2T10
	CRLF
	puts io_5
	mov ax, [esi].good_all_count
	call F2T10
	CRLF
	puts io_6
	mov ax, [esi].good_sale_count
	call F2T10
	CRLF
	puts io_7
	mov ax, [esi].good_recommand
	call F2T10
	CRLF
	puts io_8
	mov ax, [esi].good_sort
	call F2T10
	CRLF
	
	ret
outone endp

;;ax good sum, si good addr
outall proc uses ax esi goodSum:word, goodAddr:dword
	mov ax, goodSum
	mov esi, goodAddr
L7:
	;call outone
	invoke outone, esi
	add esi, sizeof goodStruct
	dec ax
	jne L7
	ret
outall endp


F2T10 proc
	push ebx
	push dx
	push esi
	lea esi, bufF2T10 ;;;;;;;
	cmp dx, 32
	je B
	movsx eax, ax
B:
	or eax, eax
	jns plus
	neg eax
	mov byte ptr [esi], '-'
	inc esi
plus:
	mov ebx, 10
	call RADIX	
	mov byte ptr [esi], 0
	;lea dx, bufF2T10
	;movsx edx, dx
	mov edx, offset bufF2T10
	invoke printf, edx
	pop esi
	pop dx
	pop ebx
	ret
	; buf db 12 dup(?)
F2T10 endp

RADIX proc
	push ecx
	push edx
	xor ecx,ecx
LOP1:
	xor edx, edx
	div ebx
	push dx
	inc ecx
	or eax, eax
	jnz LOP1
LOP2:
	pop ax
	cmp al, 10
	jb LO1
	add AL, 7
LO1:
	add al ,30H
	mov [esi], al
	inc esi
	loop LOP2
	pop edx
	pop ecx
	ret
RADIX endp

;di good addr
change_good proc goodAddr:dword, SIGN:dword, DAT:dword
	mov dx, 16
	mov edi, goodAddr
ch1:
	puts io_2
	xor ax, ax
	mov al, [edi].good_sale
	call F2T10
	invoke putchar, ' '
	gets BUF1
	;call F10T2
	invoke F10T2, esi, cx, SIGN, DAT
	cmp esi, -1
	jz ch1
	mov [edi].good_sale, al
ch2:
	puts io_3
	mov ax, [edi].good_in_price
	call F2T10
	invoke putchar, ' '
	gets BUF1
	;call F10T2
	invoke F10T2, esi, cx, SIGN, DAT
	cmp si, -1
	jz ch2
	mov [edi].good_in_price, ax
ch3:
	puts io_4
	mov ax, [edi].good_out_price
	call F2T10
	invoke putchar, ' '
	gets BUF1
	;call F10T2
	invoke F10T2, esi, cx, SIGN, DAT
	cmp si, -1
	jz ch3
	mov [edi].good_out_price, ax
ch4:
	puts io_5
	mov ax, [edi].good_all_count
	call F2T10
	invoke putchar, ' '
	gets BUF1
	;call F10T2
	invoke F10T2, esi, cx, SIGN, DAT
	cmp si, -1
	jz ch4
	mov [edi].good_all_count, ax
	ret
change_good endp

end
