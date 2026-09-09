#include "keyboard.h"
#include "screen.h"

const char scancode_to_ascii[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /*29: Control*/
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* 42: Left Shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, /*54: Right Shift*/
    '*',
    0, /*56: Left Alt*/
    ' ',
};

void keyboard_callback(uint8_t scancode){
    char letter = scancode_to_ascii[scancode];
    if (letter == 0) 	{ return; }
    if (scancode > 57)	{ return; }
    char to_print[2] = {letter, 0};
    print(to_print);
}
