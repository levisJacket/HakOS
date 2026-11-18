#include "screen.h"
#include "../kernel/low_level.h"

typedef int (*char_handler_fn)(char c, int offset, char attr);
    
int handle_newline(char c, int offset, char attr) {
    int rows = offset / (2 * MAX_COLS);
    return get_screen_offset(0, rows + 1);
}

int handle_backspace(char c, int offset, char attr) {
    offset -= 2;
    ((char*)VIDEO_ADDRESS)[offset] = ' ';
    return offset;
}

int handle_regular(char c, int offset, char attr) {
    unsigned char *vidmem = (unsigned char *)VIDEO_ADDRESS;
    vidmem[offset] = c;
    vidmem[offset + 1] = attr;
    return offset + 2;
}

char_handler_fn char_map[256] = {
    [0 ... 255] = handle_regular,
    [0x08] = handle_backspace,
    [0x0A] = handle_newline,
};

void print_char(char character, char attribute_byte){
    unsigned char *vidmem = (unsigned char *) VIDEO_ADDRESS;
    if (!attribute_byte){
	attribute_byte = WHITE_ON_BLACK;
    }

    int offset = get_cursor();
    offset = char_map[(unsigned char)character](character, offset, attribute_byte);

    offset = handle_scrolling(offset);
    set_cursor(offset);
}

int get_screen_offset(int col, int row){
    return (row*MAX_COLS + col) * 2;
}

int get_cursor(){
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset*2;
}

void set_cursor(int offset){
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)offset );
}

void print_at(char* message, int col, int row) {
    if (col >= 0 && row >= 0){
	set_cursor(get_screen_offset(col,row));
    }
    int i = 0;
    while(message[i] != 0){
	print_char(message[i++], WHITE_ON_BLACK);
    }
}

void print(char* message) {
    int i = 0;
    while(message[i] != 0){
	print_char(message[i++], WHITE_ON_BLACK);
    }
}

char charmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void print_long(int val){
    char to_print[10];
    to_print[8] = '\n';
    to_print[9] = 0;
    for (int i = 0; i < 8 ; i++){
	to_print[7-i] = charmap[val & 15];
	val = val >> 4; 
    }
    print(to_print);
}

void clear_screen() {
    set_cursor(get_screen_offset(0, 0));
    
    for (int i=0; i<MAX_ROWS * MAX_COLS; i++) {
	print_char(' ', WHITE_ON_BLACK);
    }

    set_cursor(get_screen_offset(0, 0));
}

void memory_copy(char* source, char* dest, int no_bytes){
    int i;
    for (i=0; i<no_bytes; i++){
	*(dest + i) = *(source + i);
    }
}

int handle_scrolling(int cursor_offset) {
    if (cursor_offset < MAX_ROWS*MAX_COLS*2){
	return cursor_offset;
    }

    int i;
    for (i=1; i<MAX_ROWS; i++){
	memory_copy((char*)get_screen_offset(0,i)+VIDEO_ADDRESS,
		(char*)get_screen_offset(0, i-1)+VIDEO_ADDRESS,
		MAX_COLS*2
	);
    }

    char* last_line = (char*)get_screen_offset(0, MAX_ROWS-1) + VIDEO_ADDRESS;
    for (i=0; i < MAX_COLS*2; i++) {
	last_line[i] = 0;
    }

    cursor_offset -= 2*MAX_COLS;

    return cursor_offset;
}
