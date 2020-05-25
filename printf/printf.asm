; ----------------------------------------------------------------------------------------
;     nasm -felf64 printf.asm && ld printf.o && ./a.out
; ----------------------------------------------------------------------------------------

global main

section .text

main:
    
    push 127
    push '!'
    push 100
    push 3802
    push arg_str

    call printf
    add rsp, 8*5                ; remove args from stack
    
    
    mov rax, EXIT_CALL          ; system call for exit
    xor rdi, rdi                ; exit code 0
    syscall                     ; invoke operating system to exit



printf:
; --------------------------------------------------------------
; input: args in stack (CDECL)
;
; output: rbx - argument counter
; destr: rdi, rax, rbx, rcx, rdx, r8, r9, r10, r11, rsi, r12
; --------------------------------------------------------------
    push rbp                            ; stack frame
    mov rbp, rsp
    
    mov rsi, format_str
    mov rdi, rsi
    call strlen                         ; rcx = strlen

    mov r11, buf                        ; r11 = buffer address 
    
    xor rbx, rbx
    inc rbx                             ; rbx - argument counter
    

    mov rdi, rsi                        ; rdi - address of current position in format string
    parsing_loop:
        mov rsi, rdi                    ;  rsi = start address of current part (between %)
        
        cld
        mov al, '%' 
        repne scasb                     ; find % in format string, rcx = chars before string end
        
        mov r8, rdi                     ; 
        sub r8, rsi                     ; r8 = number of skipped chars
        dec r8                          ;    
        
        mov r10, rdi                     ; store rdi
        mov rdi, r11
        
        cmp rcx, 0h
        je end_loop                     ; exit loop if % not found 
        
        mov r9, rcx                     ; store cx
        
        mov rcx, r8
        
        mov al, [r10]                   ; al = printf flag
        cmp al, '%'
        
        je .write                        ; if specifier is % - no arguments
        
        lea rdx, [rbp + 8 + 8*rbx]      ; rdx = cur argument address
        inc rbx                         ; rbx++
       
        .write: 
            call write_to_buf            ; if found %

        mov r11, rdi                     ; r11 = new buf address
        mov rcx, r9                      ; restore cx
        mov rdi, r10                     ; restore rdi

        dec rcx                          ; rcx--
        inc rdi                          ; rdi++
        jmp parsing_loop

    end_loop: 
       
        inc r8                           ; r8++
        cmp r8, 0h                       ; if nothing to write
        
        je exit
        mov rcx, r8
        
        cld
        rep movsb                       ; write skipped substring
        
        
     exit:   
        mov rsi, buf 
        call write_buf
        
            
        pop rbp
        
        ret
        
check_overflow:
; -----------------------------------------------------
; Dumps buffer in case of potential overflow, 
; resets buffer address to start
; input: rsi - start address
;        rdi - current address
;        rdx - number of expected chars to write to buf
; output: rdi - new address in buffer
; destr: rax, rdi, rsi, rdx, rcx
; ----------------------------------------------------
    mov rax, rsi       ; store rsi
    
    add rdx, rdi       ; rdx = new buffer end
    add rsi, BUFSIZE   ; rsi = real buffer end
    cmp rdx, rsi        
    
    mov rsi, rax       ; restore rsi
    jbe .exit          ; if no overflow
    
    
    
    call write_buf     ; write buffer to restart
    
    mov rdi, rsi        ; rdi = rsi
    xor rax, rax        ; rax = 0h 
    mov rcx, BUFSIZE    
    rep stosb           ; clear buffer
    
    
    mov rdi, rsi       

    .exit: ret


write_to_buf:
; --------------------------------------------------------------
; input: al - flag (c/d/s/x/o/b/%)
;        rdx - address of argument (of type in al)
;        rsi - address of format string (copy from)
;        rdi - buffer address (copy to)
;        rcx - number of chars to copy
;        r12 - start of buffer
; output: rdi - new address in buffer
; destr: rcx, rsi, rdi, rax, r11, r15, r13, r14
; --------------------------------------------------------------
    mov r13, rsi        ;
    mov r14, rdx        ; store
    mov r15, rax        ; registers
    push rcx            ;
    
    mov rsi, buf
    mov rdx, rcx
    
    call check_overflow
    
    pop rcx             ;
    mov rax, r15        ; restore
    mov rdx, r14        ; registers
    mov rsi, r13        ;
    
    cld
    rep movsb           ; move skipped chars to buffer
    
    cmp al, '%'
    je percent  
    
    cmp al, 'x'         ; 
    ja exit             ; exit if 
    cmp al, 'b'         ; unknown specifier
    jb exit             ;
    
    sub al, 'b'
  

    mov rax, [jump_table + 8*rax]   ; switch (rax)
    jmp rax
    

    percent:
        mov [rdi], byte '%'
        inc rdi
        ret
    
    char:
        mov rax, [rdx]
        stosb
        ret 
        
    string:
        mov r11, rdi        ; store rdi
        
        mov rdi, [rdx]
        call strlen
        
        mov rsi, [rdx]
        mov rdi, r11        ; restore rdi
        rep movsb
        ret
        
        
    decimal:
        mov rax, [rdx]
        mov rcx, num_buf
        
        jmp print_dec_loop  
        
        
    oct:
        mov rax, [rdx]
        mov cl, 3
        mov r11, (1 << 3) - 1
        mov rsi, num_buf
        jmp print_not_dec_loop
        
    bin:
        mov rax, [rdx]
        mov cl, 1
        mov r11, (1 << 1) - 1
        mov rsi, num_buf
        jmp print_not_dec_loop
    
    hex:
        mov rax, [rdx]
        mov cl, 4
        mov r11, (1 << 4) - 1
        mov rsi, num_buf
        jmp print_not_dec_loop
    
    
    print_not_dec_loop:
        mov rdx, rax            ; store copy of initial number
        shr rax, cl             ; rax = q
        and rdx, r11            ; rdx = r
        mov dl, [dict + rdx]    ; dl = ascii code of digit
        
        mov [rsi], rdx          ; move to num_buffer
        inc rsi                 ; rsi++
        
        cmp rax, 0h             ; if no more digits
        jne print_not_dec_loop
        
        mov rdx, rsi
        dec rdx
        mov rcx, rsi
        sub rcx, num_buf
        jmp reverse_num    
    
    
    
    print_dec_loop: 
        xor rdx, rdx
        mov rsi, 10
        div rsi                  ; rax = rdx:rax / rsi, rdx = rdx:rax % rsi
        mov dl, [dict + rdx]     ; dl = ascii code of digit
        
        mov [rcx], dl            ; put to buf
        inc rcx
        
        cmp rax, 0h
        jne print_dec_loop
    
        mov rdx, rcx
        dec rdx
        sub rcx, num_buf
    
    reverse_num:                ; copy chars to buffer in correct order
        mov al, [rdx]
        mov [rdi], al
        inc rdi
        dec rdx
        loop reverse_num
    
        ret




strlen:
; --------------------------------------------------------------
; Returns length of string
;
; input: rdi - address of string
; output: rcx
; destr: rax, rcx, rdi
; --------------------------------------------------------------
    cld

    xor rax, rax            ; rax = 0

    xor rcx, rcx            
    not rcx                 ; rcx = MAXINT

    repne scasb             ; rcx = MAXINT - strlen

    not rcx
    dec rcx

    ret
    
    
write_buf:
; --------------------------------------------------------------
; input: rsi - address of string
;       
; output: 
; destr: rax, rdi, rsi, rdx, rcx
; --------------------------------------------------------------
    mov rdi, rsi
    call strlen                 ; rcx = strlen
    mov rax, WRITE_CALL         ; system call for write
    mov rdi, STDOUT             ; file handle 1 is stdout
    mov rdx, rcx                ; number of bytes
    syscall                     ; invoke operating system to do the write
    

section .data
    
WRITE_CALL    equ 1   
STDOUT        equ 1 
EXIT_CALL     equ 60    

BUFSIZE       equ 200
buf           times BUFSIZE db 0
num_buf       times BUFSIZE db 0
dict          db "0123456789ABCDEF", 0h

jump_table    dq bin                            ; 'b'
              dq char                           ; 'c'
              dq decimal                        ; 'd'
              times ('o' - 'd' - 1) dq exit     ; error
              dq oct                            ; 'o'
              times ('s' - 'o' - 1) dq exit     ; error
              dq string,                        ; 's'
              times ('x' - 's' - 1) dq exit     ; error
              dq hex                            ; 'x'

format_str:   db "I %s %x. %d%%%c%b", 0h
arg_str:      db "love", 0h