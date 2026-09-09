
%include "print_string.asm"

print_hex:
    mov cx, dx
    and cx, 0x000f

    mov bx, HEX_MAP
    add bx, cx
    mov al, [bx]

    mov bx, HEX_OUT
    mov [bx+5], al

    mov cx, dx
    shr cx, 4
    and cx, 0x000f

    mov bx, HEX_MAP
    add bx, cx
    mov al, [bx]

    mov bx, HEX_OUT
    mov [bx+4], al

    mov cx, dx
    shr cx, 8
    and cx, 0x000f

    mov bx, HEX_MAP
    add bx, cx
    mov al, [bx]

    mov bx, HEX_OUT
    mov [bx+3], al

    mov cx, dx
    shr cx, 12
    and cx, 0x000f

    mov bx, HEX_MAP
    add bx, cx
    mov al, [bx]

    mov bx, HEX_OUT
    mov [bx+2], al
 
    mov bx, HEX_OUT
    call print_string
    ret

HEX_MAP:
    db '0123456789ABCDEF'

HEX_OUT:
    db '0x0000', 0
