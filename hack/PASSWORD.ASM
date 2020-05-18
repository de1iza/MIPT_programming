.model tiny

.data
    BUF_SIZE        equ 20d     
    INPUT_INT       equ 0ah
    TERMINATE_INT   equ 4c00h
    DISPLAY_INT     equ 09h
    CYPHER_SHIFT    equ 21h
    CHECKSUM        equ 0bdadh

    input_buf db BUF_SIZE dup(0)

    password  db 'b' + 'w'
              db 'e' + 'i'
              db 'd' + 'l'
              db 's' + 'l'
              db 'h' + 'p'
              db 'e' + 'o'
              db 'e' + 'w'
              db 't' + 'e'
              db 's' + 'r'
              db 0dh
    password_len equ $ - password - 1
    
    access_denied   db 'Access denied! Wrong password.', 24h
    access_granted  db 'Access granted.', 24h
    hack_protection db 'You are trying to hack me!', 24h
    key             db 'willpower', 0h

    

.code
org 100h

locals @@


start:
    lea di, input_buf
    call get_password

    call check_checksum
    
    lea di, input_buf
    call check_password


    exit: mov ax, TERMINATE_INT
	int 21h


get_password proc
; -------------------------------------------------
;  Reads password from input
;  Enrty: di - buffer address to store input
; --------------------------------------------------
    xor ax, ax
    inc ah

    input_loop:
            int 21h
            stosb
            cmp al, 0dh
            je input_loop_end
            jmp input_loop

    input_loop_end:
            ret

get_password endp


check_password proc
; -------------------------------------------------
;  Checks password and grants or denies access
;  Entry: di - address of password from input
;	
;  Destr: si, bx, cx, dx
;  Return: 
; --------------------------------------------------
    lea bx, password
    mov cx, password_len
    call decypher

    lea si, password
    mov cx, password_len
    repe cmpsb 

    jne wrong_password
    
    correct_password:
        lea dx, access_granted
        call display_text
        ret

    wrong_password:
        lea dx, access_denied
        call display_text
        ret
check_password endp



check_checksum proc
; -----------------------------------------------------
;  Calculates checksum of checking password function    
;  Destr: si, cx, bx, dx, ax
; -----------------------------------------------------
    lea si, check_password
    lea cx, check_checksum
    sub cx, si

    xor bx, bx
    calc_sum:
        add bx, [si]
        inc si
    loop calc_sum

    cmp bx, CHECKSUM
    je @@exit
    
    lea dx, hack_protection
    call display_text
    jmp exit

    @@exit: ret

check_checksum endp


display_text proc
; -------------------------------------------------
;  Writes string to output
;  Entry: dx - address of string
;	    
;  Destr: ax
; --------------------------------------------------
    xor ax, ax
    mov ah, DISPLAY_INT
    int 21h
    ret
display_text endp

decypher proc
; -------------------------------------------------
;  Decyphers password 
;  Entry: bx - address of password 
;	      cx - input length
;  Destr: dx, bx, cx
; --------------------------------------------------
    xor si, si
    decypher_loop:
        mov dl, [bx]
        sub dl, key[si]
        mov [bx], dl
        inc bx
        inc si
    loop decypher_loop

    ret
decypher endp


end start
