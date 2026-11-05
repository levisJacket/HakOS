[bits 32]
[extern main]
push dword 0x7E00 ;memory map location
call main
jmp $
