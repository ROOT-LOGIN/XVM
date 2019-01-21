;.686
;.DOSSEG

;PUBLIC __cpuid
GLOBAL ___cpuid
GLOBAL ___get_memory_map

;code SEGMENT
SEGMENT code PUBLIC ALIGN=16
;__cpuid PROC NEAR C hi:WORD, lo:WORD
; bp : word
; ret : word
; buffer : word
; pid : word
___cpuid:
push bp
mov bp, sp
add bp, 4
mov bx, word [bp+2]
mov eax, dword [bx]
cpuid
mov bp, word [bp]
mov dword [bp], eax
mov dword [bp+4], ebx
mov dword [bp+8], ecx
mov dword [bp+12], edx
pop bp
ret 6
;__cpuid ENDP

; si : word
; di : word
; bp : word <- bp
; ret : word
; buffer : word
; pnum : word
___get_memory_map:
push bp
mov bp, sp
add bp, 4
push di
push si
mov di, word [bp]
mov eax, 0xe820
mov ecx, 32
mov edx, 'PAMS'
mov si, word [bp+2]
mov ebx, dword [si]
int 15h
jnc ___get_memory_map.compare_eax
mov eax, 0x44444444
jmp ___get_memory_map.return
.compare_eax:
cmp eax, 'PAMS'
jnz ___get_memory_map.return
mov dword [si], ebx
xor eax, eax
.return:
pop si
pop di
pop bp
ret

;code ENDS

;END
