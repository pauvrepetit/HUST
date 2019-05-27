.386
.model   flat,stdcall
option   casemap:none

WinMain  proto :DWORD,:DWORD,:DWORD,:DWORD
WndProc  proto :DWORD,:DWORD,:DWORD,:DWORD
Display  proto :DWORD

include      menuID.INC

include      windows.inc
include      user32.inc
include      kernel32.inc
include      gdi32.inc
include      shell32.inc

includelib   user32.lib
includelib   kernel32.lib
includelib   gdi32.lib
includelib   shell32.lib

F2T10 proto
RADIX proto
CalRecommand proto
OutputOne proto :DWORD, :DWORD, :DWORD

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

.data
ClassName    db       'TryWinClass',0
AppName      db       'Our First Window',0
MenuName     db       'MyMenu',0
DlgName	     db       'MyDialog',0
AboutMsg     db       '我是CS1706胡澳',0
hInstance    dd       0
CommandLine  dd       0

bufF2T10	db	20 dup(0)

	ga1		goodStruct	<'pen$', 3, 10, 35, 56, 70, 25, ?>
	ga2		goodStruct	<'book$', 4, 9, 12, 30, 25, 5, ?>
	ga3		goodStruct	<'disk$', 4, 7, 35, 56, 70, 25, ?>
	ga4		goodStruct	<'bag$', 3, 10, 20, 50, 1000, 0, ?>
	ga5		goodStruct	<'pencil$', 6, 8, 3, 5, 100, 30, ?>

	goodLen equ $ - ga5
	N	equ	5


	; msg_goodName		db	'商品名称', 0
	; msg_goodSale		db	'商品折扣', 0
	; msg_goodInPrice		db	'进货价',	0
	; msg_goodOutPrice	db	'销售价',	0
	; msg_goodAllCount	db	'商品总数',	0
	; msg_goodSaleCount	db	'销售数',	0
	; msg_goodRecommand	db	'推荐度',	0
	; msg_goodSort		db	'推荐度排序',0

	msg_goodName		db	'good', 		0
	msg_goodSale		db	'sale', 			0
	msg_goodInPrice		db	'inPrice',			0
	msg_goodOutPrice	db	'outPrice',			0
	msg_goodAllCount	db	'allCount',			0
	msg_goodSaleCount	db	'saleCount',		0
	msg_goodRecommand	db	'recommandIndex',	0
	msg_goodSort		db	'recommandSort',	0

.code
Start:	     
		invoke GetModuleHandle,NULL
		mov    hInstance,eax
		invoke GetCommandLine
		mov    CommandLine,eax
		invoke WinMain,hInstance,NULL,CommandLine,SW_SHOWDEFAULT
		invoke ExitProcess,eax
		;;
WinMain	proc   hInst:DWORD,hPrevInst:DWORD,CmdLine:DWORD,CmdShow:DWORD
		LOCAL  wc:WNDCLASSEX
		LOCAL  msg:MSG
		LOCAL  hWnd:HWND
		invoke RtlZeroMemory,addr wc,sizeof wc
		mov    wc.cbSize,SIZEOF WNDCLASSEX
		mov    wc.style, CS_HREDRAW or CS_VREDRAW
		mov    wc.lpfnWndProc, offset WndProc
		mov    wc.cbClsExtra,NULL
		mov    wc.cbWndExtra,NULL
		push   hInst
		pop    wc.hInstance
		mov    wc.hbrBackground,COLOR_WINDOW+1
		mov    wc.lpszMenuName, offset MenuName
		mov    wc.lpszClassName,offset ClassName
		invoke LoadIcon,NULL,IDI_APPLICATION
		mov    wc.hIcon,eax
		mov    wc.hIconSm,0
		invoke LoadCursor,NULL,IDC_ARROW
		mov    wc.hCursor,eax
		invoke RegisterClassEx, addr wc
		INVOKE CreateWindowEx,NULL,addr ClassName,addr AppName,\
				WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,\
				CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,\
				hInst,NULL
		mov    hWnd,eax
		INVOKE ShowWindow,hWnd,SW_SHOWNORMAL
		INVOKE UpdateWindow,hWnd
		;;
MsgLoop:     
		INVOKE GetMessage,addr msg,NULL,0,0
		cmp    EAX,0
		je     ExitLoop
		INVOKE TranslateMessage,addr msg
		INVOKE DispatchMessage,addr msg
		jmp    MsgLoop 
ExitLoop:    
		mov    eax,msg.wParam
	    ret
WinMain      endp

WndProc      proc   hWnd:DWORD,uMsg:DWORD,wParam:DWORD,lParam:DWORD
	LOCAL  hdc:HDC
	.IF     uMsg == WM_DESTROY
		invoke PostQuitMessage,NULL
	.ELSEIF uMsg == WM_COMMAND
		.IF     wParam == IDM_FILE_EXIT
			invoke SendMessage,hWnd,WM_CLOSE,0,0
		.ELSEIF wParam == IDM_ACTION_RECOMMAND
			invoke CalRecommand
		.ELSEIF wParam == IDM_ACTION_LIST
			invoke Display,hWnd
		.ELSEIF wParam == IDM_HELP_ABOUT
			invoke MessageBox,hWnd,addr AboutMsg,addr AppName,0
		.ENDIF
	.ELSE
		invoke DefWindowProc,hWnd,uMsg,wParam,lParam
		ret
	.ENDIF
		xor    eax,eax
		ret
WndProc      endp

Display      proc   hWnd:DWORD
			XX     equ  10
			YY     equ  10
			XX_GAP equ  100
			YY_GAP equ  30
			LOCAL  hdc:HDC
			invoke GetDC,hWnd
			mov    hdc,eax

			invoke TextOut, hdc, XX + 0 * XX_GAP, YY + 0 * YY_GAP, offset msg_goodName, 4
			invoke TextOut, hdc, XX + 1 * XX_GAP, YY + 0 * YY_GAP, offset msg_goodSale, 4
			invoke TextOut, hdc, XX + 2 * XX_GAP, YY + 0 * YY_GAP, offset msg_goodInPrice, 7
			invoke TextOut, hdc, XX + 3 * XX_GAP, YY + 0 * YY_GAP, offset msg_goodOutPrice, 8
			invoke TextOut, hdc, XX + 4 * XX_GAP, YY + 0 * YY_GAP, offset msg_goodAllCount, 8
			invoke TextOut, hdc, XX + 5 * XX_GAP, YY + 0 * YY_GAP, offset msg_goodSaleCount, 9
			invoke TextOut, hdc, XX + 6 * XX_GAP, YY + 0 * YY_GAP, offset msg_goodRecommand, 14

			lea esi, ga1

			mov edx, YY + YY_GAP
			invoke TextOut, hdc, XX + 0 * XX_GAP, edx, esi, byte ptr [esi + 10]
			mov edx, YY + YY_GAP
			invoke OutputOne, 11, 1, hdc
			invoke OutputOne, 12, 2, hdc
			invoke OutputOne, 14, 3, hdc
			invoke OutputOne, 16, 4, hdc
			invoke OutputOne, 18, 5, hdc
			invoke OutputOne, 20, 6, hdc

			add esi, goodLen
			mov edx, YY + 2 * YY_GAP
			invoke TextOut, hdc, XX + 0 * XX_GAP, edx, esi, byte ptr [esi + 10]
			mov edx, YY + 2 * YY_GAP
			invoke OutputOne, 11, 1, hdc
			invoke OutputOne, 12, 2, hdc
			invoke OutputOne, 14, 3, hdc
			invoke OutputOne, 16, 4, hdc
			invoke OutputOne, 18, 5, hdc
			invoke OutputOne, 20, 6, hdc

			add esi, goodLen
			mov edx, YY + 3 * YY_GAP
			invoke TextOut, hdc, XX + 0 * XX_GAP, edx, esi, byte ptr [esi + 10]
			mov edx, YY + 3 * YY_GAP
			invoke OutputOne, 11, 1, hdc
			invoke OutputOne, 12, 2, hdc
			invoke OutputOne, 14, 3, hdc
			invoke OutputOne, 16, 4, hdc
			invoke OutputOne, 18, 5, hdc
			invoke OutputOne, 20, 6, hdc

			add esi, goodLen
			mov edx, YY + 4 * YY_GAP
			invoke TextOut, hdc, XX + 0 * XX_GAP, edx, esi, byte ptr [esi + 10]
			mov edx, YY + 4 * YY_GAP
			invoke OutputOne, 11, 1, hdc
			invoke OutputOne, 12, 2, hdc
			invoke OutputOne, 14, 3, hdc
			invoke OutputOne, 16, 4, hdc
			invoke OutputOne, 18, 5, hdc
			invoke OutputOne, 20, 6, hdc

			add esi, goodLen
			mov edx, YY + 5 * YY_GAP
			invoke TextOut, hdc, XX + 0 * XX_GAP, edx, esi, byte ptr [esi + 10]
			mov edx, YY + 5 * YY_GAP
			invoke OutputOne, 11, 1, hdc
			invoke OutputOne, 12, 2, hdc
			invoke OutputOne, 14, 3, hdc
			invoke OutputOne, 16, 4, hdc
			invoke OutputOne, 18, 5, hdc
			invoke OutputOne, 20, 6, hdc

			ret
Display      endp

OutputOne proc countOut:DWORD, countLocate:DWORD, hdc:DWORD
	pushad
	mov ebx, countOut
	xor eax, eax
	cmp ebx, 11
	jz number1
	mov ax, [esi + ebx]
	jmp next1
number1:
	mov al, [esi + ebx]
next1:
	invoke F2T10
	mov ecx, countLocate
	imul ecx, XX_GAP
	add ecx, XX
	invoke TextOut, hdc, ecx, edx, offset bufF2T10, eax
	popad
	ret
OutputOne endp

CalRecommand proc
	pushad
	
    mov esi, offset ga1
	lea ebx, [esi]
	sub ebx, goodLen

	mov di, N
	inc di

calLoop:
	dec di
	add ebx, goodLen
	cmp di, 0
	jz calEnd

	mov ax, [ebx + 12]
	mov cx, 1280
	mul cx
	push dx
	push ax
	mov al, byte ptr [ebx + 14]
	mov cl, [ebx + 11]
	mul cl
	mov cx, ax
	pop ax
	pop dx
	div cx
	push ax

	mov ax, [ebx + 18]
	mov cx, 64
	mul cx
	mov cx, [ebx + 16]
	div cx
	mov dx, ax
	pop ax
	add ax, dx

	mov [ebx + 20], ax
	jmp calLoop

calEnd:
	popad
	ret
CalRecommand endp

F2T10 proc
	push ebx
	push edx
	push esi
	mov edx, 32
	lea esi, bufF2T10
	movsx eax, ax

	or eax, eax
	jns plus
	neg eax
	mov byte ptr [esi], '-'
	inc esi
plus:
	mov ebx, 10
	invoke RADIX
	mov byte ptr [esi], '$'

	lea edx, bufF2T10
	sub esi, edx
	mov eax, esi
	pop esi
	pop edx
	pop ebx
	ret
F2T10 endp

RADIX proc 
	push ecx
	push edx
	xor ecx,ecx
LOP1:
	xor edx, edx
	div ebx
	push edx
	inc ecx
	or eax, eax
	jnz LOP1
LOP2:
	pop eax
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

end  Start