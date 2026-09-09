gdt_start:

gdt_null:	;null descriptor
    dd 0x0
    dd 0x0

gdt_code:	;code segment descriptor
    ;base 0x0 limit 0xfffff
    ;flags (present) 1 (privilege) 00 (desciptor type) 1 -> 1001b
    ;type flags - (code) 1 (conforming) 0 (readable) 1 (accessed) 0 -> 1010b
    ;flags (granularity) 1 (32-bit default) 1 (64-bit seg) 0 (AVL) 0 -> 1100b
    dw 0xffff		;limit bit 0-15
    dw 0x0		;base bit 0-15
    db 0x0		;base bit 16-23
    db 10011010b	;flags & type flags
    db 11001111b	;flags & limit bit 16-19
    db 0x0		;base bit 24-31

gdt_data:		;data segment descriptor
    ;same except type flag
    ;type flags - (code) 0 (expand down) 0 (writable) 1 (accessed) 0 -> 0010b
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b	
    db 11001111b	
    db 0x0

gdt_end:	

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start 

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
