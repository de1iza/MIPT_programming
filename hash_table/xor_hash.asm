.686
.model tiny, c
.code


_XORHashAsm proc string:ptr byte, len:dword
	mov ecx, len
	mov edx, dword ptr string

	xor eax, eax			; eax = 0
	xor ebx, ebx			; ebx = 0
	hash_loop:
		mov bl, [edx]		; bl = str[i]
		xor eax, ebx		; eax ^= ebx
		rol eax, 1			; eax circular shift right 1
		inc edx				; edx++
		loop hash_loop		

	ret
_XORHashAsm endp

end