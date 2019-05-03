public outone, sort, outall, change_good
extrn io_1:byte, io_2:byte, io_3:byte, io_4:byte, io_5:byte, io_6:byte, io_7:byte, io_8:byte ,F10T2:far, BUF1:byte, bufF2T10:byte, auth:byte
.386
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
	none1			DD	?
	none2			DD	?
	none3			DD	?
	good_real_in_price	DW	?
	none4			DD	?
	none5			DD	?
	none6			DD	?
goodStruct ends

;output string$
puts macro ofset:req
    push ax
    push dx
    lea dx, ofset
    mov ah, 9
    int 21H
    pop dx
	pop ax
endm

putchar macro char:req
	push ax
	push dx
	mov dl, char
	mov ah, 2
	int 21h
	pop dx
	pop ax
endm

;input change cx si,cx is length, si is addr 
gets macro ofset:req
    push ax
    push dx
    lea dx, ofset
    mov ah, 0AH
    mov si, dx
	int 21h
	inc si
	mov cl, [si]
	xor ch, ch
	inc si
	CRLF
	pop dx
	pop ax
endm

CRLF macro
	push ax
	push dx
	mov ah, 2
	mov dl, 0AH
	int 21H
	mov dl, 0DH
	int 21H
	pop dx
	pop ax
endm

code1 segment use16
	assume cs:code1
;ax good sum, si good addr
sort proc far
	push bx
	push cx
	push dx
	jmp L3
L4:	
	dec ax
	add si, sizeof goodStruct
	cmp ax, 1
	je exit
L3:
	mov dx, [si].good_recommand
	mov bx, si
	mov cx, ax
L5:	
	add bx, sizeof goodStruct
	dec cx
	cmp dx, [bx].good_recommand
	ja	L1 
	jb	L2
next:   
	cmp cx, 1
	je L4
	jmp L5
L1:	
	inc [bx].good_sort
	jmp next
L2:
	inc [si].good_sort
	jmp next
exit:
	pop dx
	pop cx
	pop bx
	ret
sort endp

; si good addr
outone proc far
	push eax
	push dx
	push bx

	mov dx, 16
L6:
	puts io_1
	puts [si].good_name
	CRLF
	puts io_2
	xor ax, ax
	mov al, [si].good_sale
	call F2T10
	CRLF
	mov bx, offset auth
	mov bl, [bx]
	cmp bl, 0
	jz notAuth

	puts io_3
	
	cli		;堆栈检查反跟踪
	mov ax, offset pass2
	push ax
	pop ax

	mov ax, [si + 20H]
	sub ax, 5
	shr ax, 1

	mov bx, [esp - 2]
	sti
	mov cx, offset pass2
	cmp cx, bx
	jnz error
	jmp bx		;堆栈检查反跟踪

pass2:
	call F2T10
	CRLF

notAuth:
	puts io_4
	mov ax, [si].good_out_price
	call F2T10
	CRLF
	puts io_5
	mov ax, [si].good_all_count
	call F2T10
	CRLF
	puts io_6
	mov ax, [si].good_sale_count
	call F2T10
	CRLF
	puts io_7
	mov ax, [si].good_recommand
	call F2T10
	CRLF
	puts io_8
	mov ax, [si].good_sort
	call F2T10
	CRLF
	
	pop bx
	pop dx
	pop eax
	ret
outone endp

error:
	mov ax, 4C00H
	int 21H

;;ax good sum, si good addr
outall proc far
L7:
	call outone
	add si, sizeof goodStruct
	dec ax
	jne L7
	ret
outall endp


F2T10 proc far
	push ebx
	push dx
	push si
	lea si, bufF2T10 ;;;;;;;
	cmp dx, 32
	je B
	movsx eax, ax
B:
	or eax, eax
	jns plus
	neg eax
	mov byte ptr [si], '-'
	inc si
plus:
	mov ebx, 10
	call RADIX	
	mov byte ptr [si], '$'
	lea dx, bufF2T10
	mov ah, 9
	int 21H
	pop si
	pop dx
	pop ebx
	ret
	; buf db 12 dup(?)
F2T10 endp

RADIX proc
	push cx
	push edx
	xor cx,cx
LOP1:
	xor edx, edx
	div ebx
	push dx
	inc cx
	or eax, eax
	jnz LOP1
LOP2:
	pop ax
	cmp al, 10
	jb LO1
	add AL, 7
LO1:
	add al ,30H
	mov [si], al
	inc si
	loop LOP2
	pop edx
	pop cx
	ret
RADIX endp

;di good addr
change_good proc far
	mov dx, 16
ch1:
	puts io_2
	xor ax, ax
	mov al, [di].good_sale
	call F2T10
	putchar ' '
	gets BUF1
	call F10T2
	cmp si, -1
	jz ch1
	mov [di].good_sale, al
ch2:
	puts io_3
	mov ax, [di].good_in_price
	call F2T10
	putchar ' '
	gets BUF1
	call F10T2
	cmp si, -1
	jz ch2
	mov [di].good_in_price, ax
ch3:
	puts io_4
	mov ax, [di].good_out_price
	call F2T10
	putchar ' '
	gets BUF1
	call F10T2
	cmp si, -1
	jz ch3
	mov [di].good_out_price, ax
ch4:
	puts io_5
	mov ax, [di].good_all_count
	call F2T10
	putchar ' '
	gets BUF1
	call F10T2
	cmp si, -1
	jz ch4
	mov [di].good_all_count, ax
	ret
change_good endp

code1 ends
end
