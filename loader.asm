
ORG 0x7c00
BITS 16

; first copy me self to cs:0x0600
mov ax, cs
mov ss, ax
mov sp, 0x7c00
mov es, ax
mov ds,ax
mov si, 0x7c00
mov di, 0x600
mov cx, 0x200
cld
rep movsb
; set cs:ip = 0x0: 0x61c
push ax
push word 0x61c
retf
sti

; now read 8-sectors ( 4K = 512 * 8) to 0:0x7c00
xor ax, ax
mov es, ax
mov bx, 0x7c00
mov al, 8
mov ah, 2
mov cl, 2
mov ch, 0
mov dx, 0
int 0x13

; execute
xor ax, ax
push ax
push 0x7c00
retf
