#include "keyboard.h"
#include "screen.h"

uint8_t keyboard_state = 0b00000000 ;

// Zeroes are Escape, Ctrl, L Shift, R Shift, Alt in order 
const char keymap[2][128] = {
    {
	0,       27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t',   'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',
	0,'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	0, '*', 
	0, ' ',
    }, {
	0,       27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t',   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';','\'', '`',
	0,'\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
	0, '*',
	0, ' ',
    }
};

void keyboard_callback(uint8_t scancode){
    char release = scancode & 0x80;
    char type = scancode & 0x7F;
    char letter = keymap[keyboard_state & 1][type];
    char to_print[2] = {'0', 0};

    if (letter == 0){
	if (type == 42||type == 54) {
	    keyboard_state ^= 0x01;
	}
	return;
    }

    if (release) {
	
    } else {
	to_print[0] = letter;
	print(to_print);
    }
}
