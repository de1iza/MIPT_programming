.model tiny
.code
org 100h

MAXLEN equ 1000

locals @@

start:
    ; Checking if memchr works
        ;lea di, msg
        ;mov al, 'P'
        ;mov cx, 5

        ;call memchr
        ;mov ah, 02h
        ;mov dl, [si]
        ;int 21h
    ; 


    ; Checking if memset works
    ;    mov al, 0AAh
    ;    mov cx, 10
        
    ;    call memset
    ; 

     ; Checking if memcpy works
     ;   lea si, msg
     ;   mov cx, 10
     ;   lea di, msg
     ;   add di, 16h
        
     ;   call memcpy
    ; 

     ; Checking if memcmp works
        lea si, msg
        mov cx, 100
        lea di, msg1
        call memcmp
    ; 

    ; Checking if strlen works
    ;    lea di, msg   
    ;    call strlen
    ; 

    ; Checking if strchr works
    ;    lea di, msg
    ;    mov al, 4Fh
    ;    call strrchr
    ; 

    ; Checking if strcpy works
    ;    lea di, msg
    ;    lea si, msg
    ;    call strcpy
    ; 

	mov ax, 4c00h
	int 21h

memchr proc
; --------------------------------------------------------------
; void* memchr( const void* ptr, int ch, size_t count );
;                           di       al         cx
;
; return: si (pointer to the first occurence, 0 if not found)
; --------------------------------------------------------------
    cld
    mov si, 0
    repne scasb     ; zf == 1 if found
    jnz exit

    dec di
    mov si, di

    exit:    ret
memchr endp

memset proc
; --------------------------------------------------------------
; void* memset( const void* dest, int ch, size_t count );
;                           di        al         cx
;
; return: di
; destr: bx
; --------------------------------------------------------------
    cld
    mov bx, di

    rep stosb

    mov di, bx
    ret
memset endp

memcpy proc
; --------------------------------------------------------------
; void* memcpy( void* dest, const void* src, size_t count );
;                     di                si          cx
;
; return: di
; destr: 
; --------------------------------------------------------------
    cld
    rep movsb

    ret
memcpy endp

memcmp proc
; --------------------------------------------------------------
; int memcmp( const void* lhs, const void* rhs, size_t count );
;                         si               di          cx
;
; return: ax (difference between first non-mathing chars)
; destr: bl
; --------------------------------------------------------------
    cld
    repe cmpsb

    dec si  
    dec di
    mov al, [si]
    mov bh, 0h
    mov bl, [di]
    sub ax, bx
    ret
memcmp endp

strlen proc
; --------------------------------------------------------------
; size_t strlen( const char* str );
;                            di
;
; return: cx (cx = 0 if strlen > MAXLEN, else cx > 0)
; destr: al cx
; --------------------------------------------------------------
    cld
    mov al, 0h              ; al = '\0'
    mov cx, MAXLEN          ; cx = MAXLEN
    repne scasb             ; cx = MAXLEN - strlen
    jne @@exit              ; if zx == 0 exit (cx = 0)

    neg cx                  ; cx = -(MAXLEN - strlen)
    add cx, MAXLEN - 1      ; cx += MAXLEN - 1

    sub di, cx
    dec di

    @@exit: ret
    
strlen endp

strchr proc
; -------------------------------------------------------------------------------
; char *strchr( const char *str, int ch );
;                           di       al
;
; return: cx (cx = 0 if not found, else: cx = address (of char from al) in string)
; destr: dx si
; -------------------------------------------------------------------------------
    cld
    push ax
    call strlen     ;lodsb!
    pop ax

    mov dx, cx      ; dx = strlen

    cld
    mov si, di      ; si = di (pointer to str)
    repne scasb
    
    jne @@exit

    neg cx
    add cx, dx
    dec cx
    add cx, si

    @@exit: ret
strchr endp

strrchr proc
; -------------------------------------------------------------------------------
; char *strrchr( const char *str, int ch );
;                           di       al
;
; return: cx (cx = 0 if not found, else: cx = address (of char from al) in string)
; destr: dx si
; -------------------------------------------------------------------------------
    cld
    push ax
    call strlen
    pop ax

    mov dx, cx      ; dx = strlen

    std
    mov si, di      
    add di, dx     ; si = di + strlen (pointer to end of str)
    repne scasb
    
    jne @@exit

    neg cx
    add cx, dx
    dec cx
    add cx, si

    @@exit: ret
strrchr endp

strcpy proc
; -------------------------------------------------------------------------------
; char* strcpy( char* dest, const char* src );
;                     di                si
; return: di (initial dest)
; destr: cx 
; -------------------------------------------------------------------------------
    push di

    mov di, si
    call strlen
    inc cx
    mov si, di

    pop di

    cld
    rep movsb
    @@exit: ret
strcpy endp

strcmp proc
; -------------------------------------------------------------------------------
; int strcmp( const char *lhs, const char *rhs );       -strlen!
;                         si               di
; return: ax (difference between first non-mathing chars)
; destr: bx cx
; -------------------------------------------------------------------------------
    call strlen
    mov ax, cx          ; ax = strlen(di)
    mov bx, di          ; bx = di
    mov di, si          ; di = si
    call strlen         ; cx = strlen(si)

    cmp ax, cx

    mov si, bx          ; si = bx (di)
    xchg si, di
        
    call memcmp  

    ret
strcmp endp


msg db 'HELLO', 0h
msg1 db 'WORLD', 0h

end start



