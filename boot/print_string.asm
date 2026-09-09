   
print_string:
    mov ah, 0x0e
print_string_loop:
    mov al, [bx]
    test al, al         
    jz print_string_end

    mov al, [bx]
    int 0x10
    add bx, 1
    jmp print_string_loop
print_string_end:
    ret
