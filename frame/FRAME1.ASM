.model tiny
.code
org 100h

VIDEOSEG 			equ 0b800h

FRAME_COLOUR 		equ 35h
SHADOW_COLOUR		equ 10h


HOR_BORDER_C		equ 205d
VERT_BORDER_C		equ 186d
TOP_L_CORNER_C		equ 201d
TOP_R_CORNER_C		equ 187d
BOTTOM_L_CORNER_C	equ 200d
BOTTOM_R_CORNER_C	equ 188d
FRAME_FILL_C		equ ' '
MAXLEN				equ 100d
MSG_LEN 			equ 14d

ARGS_NUM			equ 4d


start:


	; frame1.com <width> <height> <x-coord-start> <y-coord-start>



	mov cx, ARGS_NUM
	call get_args

	xor ax, ax
	xor cx, cx
	xor dx, dx

	mov al, cs:[bx][0]				; al = <y-coord-start>
	mov cl, 80d
	mul cx							; ax *= 80
	
	mov dl, cs:[bx][1]				; cl = <x-coord-start>
	add ax, dx						
	shl ax, 1						; AX = FRAME_START_POINT
	frame_start_point dw ?
	mov frame_start_point, ax

	xor ax, ax
	mov al, cs:[bx][2]				; AX = HEIGHT
	frame_height dw ?
	mov frame_height, ax

	xor dx, dx
	mov dl, cs:[bx][3]				; DX = WIDTH
	frame_width dw ?
	mov frame_width, dx

	
	; Frame animation (5 frames)
	; ------------------------------------------------

	; tiny frame 20x5
	mov di, frame_start_point

	mov cx, frame_width
	shr cx, 1
	
	mov si, frame_height
	shr si, 1

	mov ax, (FRAME_COLOUR shl 8) or FRAME_FILL_C
	mov bx, ax
	mov dx, ax

	call draw_frame
	call delay

	mov di, frame_start_point
	mov ax, (FRAME_COLOUR shl 8) or FRAME_FILL_C
	mov bx, ax
	mov dx, ax
	mov cx, frame_width
	mov si, frame_height

	call draw_frame
	call draw_shadow
	; ---------------------------------------------------------

	
	; Writing message in the frame
	; ---------------------------------------------------------
	mov cx, MSG_LEN
	lea si, msg
	mov di, frame_start_point

	mov ax, frame_width
	shr ax, 1
	and al, 0FEh

	add di, ax

	call write_text
	; ---------------------------------------------------------
	
	mov ax, 4c00h
	int 21h



draw_shadow proc
; -------------------------------------------------------
; Draws horizontal and vertical shadows for a frame
; -------------------------------------------------------
	mov ax, frame_height
	mov bx, 80d
	mul bx
	inc ax
	shl ax, 1
	mov bx, frame_start_point
	add ax, bx
	mov di, ax						;di = frame_start_point + 2*(80*frame_height + 1)


	mov cx, frame_width
	mov ax, (SHADOW_COLOUR shl 8) or FRAME_FILL_C
	mov bx, (SHADOW_COLOUR shl 8) or FRAME_FILL_C
	mov dx, (SHADOW_COLOUR shl 8) or FRAME_FILL_C
	call draw_row	
	

	mov bx, ax									; save ax

	mov ax, frame_width
	shl ax, 1
	mov di, frame_start_point
	add di, ax									; di = frame_start_point + 2*frame_width 


	mov ax, bx							

	mov cx, 0
	vert_line_loop:
		cld
		stosw								; es:[di++] = ax
		add di, 2*(80 - 1)					; di += 158
		inc cx								; cx++
		cmp cx, frame_height				; if (cx != frame_height)
		jb vert_line_loop					; continue 



	ret
draw_shadow endp

write_text proc
; ---------------------------------------------
; Writes text
; Entry:
; 	  cx - text length
;	  si - source string address
;	  di - destination address
;	
;  Destr:  
; ---------------------------------------------
	cld 			; df = 0

	write_letter:
		lodsb
		mov ah, FRAME_COLOUR		
		stosw
	loop write_letter

	ret
write_text endp


.calc_offset_di macro 
; -------------------------------------------------------------------
; Adds offset to di to start drawing new line from the proper place  
; ------------------------------------------------------------------
	add di, 2*80
endm

draw_frame proc
; ---------------------------------------------
; Draws frame, including its borders
; Entry:
;	  cx - row length
;     di - address of line start
;     si - frame height
;  Destr:  si
; ---------------------------------------------
	mov bx, VIDEOSEG	
	mov es, bx

	

	mov ax, (FRAME_COLOUR shl 8) or TOP_L_CORNER_C
	mov bx, (FRAME_COLOUR shl 8) or HOR_BORDER_C
	mov dx, (FRAME_COLOUR shl 8) or TOP_R_CORNER_C
	call draw_row
	.calc_offset_di

	mov ax, (FRAME_COLOUR shl 8) or VERT_BORDER_C
	mov bx, (FRAME_COLOUR shl 8) or FRAME_FILL_C
	mov dx, (FRAME_COLOUR shl 8) or VERT_BORDER_C


	sub si, 2

	draw_rows:
    	call draw_row
		.calc_offset_di
		dec si
		cmp si, 0
		ja draw_rows

	mov ax, (FRAME_COLOUR shl 8) or BOTTOM_L_CORNER_C
	mov bx, (FRAME_COLOUR shl 8) or HOR_BORDER_C
	mov dx, (FRAME_COLOUR shl 8) or BOTTOM_R_CORNER_C
	call draw_row

	ret
draw_frame endp

draw_row proc
; ---------------------------------------------
; Draws row with different first and last chars
; Entry:
;	  ah - char to start line with
; 	  al - colour attr
;
;	  bh - char to fill line with
; 	  bl - colour attr
;
;	  dh - char to finish line with
; 	  dl - colour attr
;
;	  cx - row width
;     di - address of line start
; Destr:
; ---------------------------------------------
	push di
	push ax	; USE SI INSTEAD 
	push cx

    sub cx, 2		; cx -= 2 	
	cld
    stosw			; es:[di++] = ax
    mov ax, bx		; ax = bx
	cld
    rep stosw		; while (--cx) es:[di++] = ax;
    mov ax, dx		; ax = dx
    stosw			; es:[di++] = ax

	pop cx
	pop ax
	pop di
	ret
draw_row endp

delay proc   
  mov cx, 0
  mov dx, 07120h 
  mov ah, 86h    
  int 15h
  ret
delay endp 

strlen proc
	; --------------------------------------------------------------
	; size_t strlen( const char* str );
	; Entry:                     di		  cx - maxlen    al - ending char
	;
	; return: cx (cx = 0 if strlen > MAXLEN, else cx > 0)
	; destr: al cx bx
	; --------------------------------------------------------------
	cld
	;mov al, 20h             ; al = ' '
	mov bx, cx         		; bx = MAXLEN
	repne scasb             ; cx = MAXLEN - strlen
	jne @@exit              ; if zx == 0 exit (cx = 0)

	neg cx                  ; cx = -(MAXLEN - strlen)
	add cx, bx          	; cx += MAXLEN - 1
	dec cx

	sub di, cx
	dec di

	@@exit: ret

strlen endp

get_num_arg proc
	; -------------------------------------------------
	; Reads and parses numeric argument from cmd
	; Entry:
	;	si - address of arg
	;	al - ending char
	;		
	;
	;  Destr: di ax bx cx dx
	;  Return: al	-	value of arg
	; --------------------------------------------------
	mov di, si

	xor cx, cx
	xor bx, bx
	mov bl, 80h
	mov cl, [bx]				

	call strlen
	xor ax, ax

	@@read_char:
		mov bx, 10d			;
		mul bx				;  ax = ax*10

		xor dx, dx

		mov dl, [si]
		inc si 

		sub dl, 30h

		add al, dl			
	loop @@read_char
	
	ret
get_num_arg endp

get_args proc
	; -------------------------------------------------
	; Reads and parses all arguments from cmd
	; Entry:
	;	cx - expected number of args
	;	
	;  Destr: ax bx cx si di
	;  Return: bx - address of array with parsed args
	; --------------------------------------------------
	xor ax, ax
	xor bx, bx

	mov si, 82h

	dec cx

	array db ARGS_NUM dup(?)

	parse_arg:
		push cx
		mov al, ' '
		call get_num_arg
		pop cx				; get loop cnt back
	
		mov bx, cx
		mov byte ptr array[bx], al 		; save value returned from func

		inc si
	loop parse_arg


	mov al, 0dh
	call get_num_arg
		
	mov byte ptr array[0], al 		; STORE IN REVERSE ORDER IN MEMORY

	lea bx, array

	ret
get_args endp

msg: db 'Hello, world!', 0h

exit: end start
