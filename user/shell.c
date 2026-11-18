#include "shell.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../kernel/process.h"

#include "dummy.h"

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

typedef void (*command_fn)(int argc, char **argv);

typedef struct {
    const char *name;
    command_fn func;
} command_entry;

command_entry command_list[];

void cmd_help(int argc, char **argv){
    for(int i=0; command_list[i].name; i++){
	print(command_list[i].name);
extern void dummyProc();
extern void dummyProc();
	print("\n");
    }
}

void cmd_clear(int argc, char **argv){
    clear_screen();
}

void cmd_echo(int argc, char **argv){
    if (argc > 1)	print(argv[1]);
}

void cmd_new(int argc, char **argv){
    process_create(dummyProc);
}

command_entry command_list[] = {
    {"help",cmd_help},
    {"clear",cmd_clear},
    {"echo",cmd_echo},
    {"new", cmd_new},
    {0,0},
};

void shell_execute(char *line_buffer) {
    int argc = 0;
    char* argv[16];
    char block = 0;
    
    for (int i=0; line_buffer[i]; i++) {
	if (line_buffer[i] == ' '){
	    line_buffer[i] = 0;
	    block = 0;
	} else if(!block){
	    block = 1;
	    argv[argc++] = &line_buffer[i];
	}
    }
    
    print("\n");

    for(int i=0; command_list[i].name; i++){
	if (strcmp(argv[0], command_list[i].name) == 0){
	    command_list[i].func(argc,argv);
	    print("\n");
	    return;
	}
    }
    print(argv[0]);
    print(": command not found\n");
}

void shell_loop(){
    char line_buffer[256];
    unsigned char buffer_index = 0;

    while(1){
	char next_letter = getchar();

	if(next_letter){
	    if(next_letter == '\n'){
		line_buffer[buffer_index] = 0;
		shell_execute(line_buffer);
		buffer_index = 0;
	    } else if (next_letter == '\b') {
		if (buffer_index) { buffer_index--; }
	    } else {
		line_buffer[buffer_index++] = next_letter;
	    }
	}
    }
}
