#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ops.h"
#include "stack.h"
#include "machine.h"

#define STACK_LENGTH 128

// TODO: load programu, format instrukcji: 2 słowa w linijce - instruckja i wartość
// TODO: kompilator - zamnienia assembler na bajtkod i dodaje go do swojego kodu - generuje plik wykonywalny z aplikacją vm i wbudowanym kodem

stack stack_new(uint8_t size) {
    stack stack;
    stack.sp = 0;
    stack.size = size;
    stack.stack = malloc(size * sizeof(uint8_t));
    return stack;
}

uint8_t stack_push(stack* obj, uint8_t new_item) {
    obj->stack[obj->sp++] = new_item;
    return obj->sp;
}

uint8_t stack_pop(stack *obj) {
    return obj->stack[--(obj->sp)];
}

void machine_execute(machine* obj) {    
    while(machine_load_data_next(obj));
    while(machine_execute_next(obj));
}

bool machine_load_data_next(machine* obj) { // returns true if we're to keep loading data
    uint8_t piece_of_data = obj->program[obj->pc/2];
    if (piece_of_data == 0) {
	obj->pc+=2;
	return false;
    }
    
    obj->pc += 2;
    
    stack_push(&(obj->data), piece_of_data);
    return true;
}

bool machine_execute_next(machine* obj) { // returns 1 if running shoud be continued, 0 otherwise
    uint8_t instruction;
    
    if(0 == obj->pc % 2) {
	instruction = (obj->program[obj->pc/2] & 0xf0) >> 4;
	opcodes[instruction](obj);
    } else {
	instruction = obj->program[(obj->pc-1)/2] & 0x0f;
	opcodes[instruction](obj);
    }
    ++obj->pc;
    
    return obj->running;
}

void usage() {
    printf("usage: dupsko <file>\n");
    exit(1);
}

uint8_t* load_file(char* filename) {
    FILE* f;
    uint8_t* code = NULL;
    struct stat st;
    
    if ((f = fopen(filename, "r"))) {
	fstat(fileno(f), &st);
	code = malloc(st.st_size);
	fread(code, 1, st.st_size, f);
	fclose(f);
    } else {
	printf("ERROR: invalid file %s", filename);
	usage();
    }

    return code;
}

void write_file(char* filename, uint8_t* buffer, size_t len) {
    FILE* f;

    if ((f = fopen(filename, "wb"))) {
	fwrite(buffer, sizeof(uint8_t), len, f);
	fclose(f);
    } else {
	printf("ERROR: invalid file %s", filename);
	usage();
    }
}

void append_opcode(uint8_t* buffer, uint64_t* counter, uint8_t opcode) {
    if(0 == (*counter % 2)) {
	buffer[*counter/2] = 0;
	buffer[*counter/2] = opcode<<4;
    } else {
	buffer[(*counter-1)/2] |= opcode;
    }
    ++*counter;
}

int main(int argc, char** argv) {
    char c;
    bool compile = true;
    char* input = NULL;
    char* output = NULL;

    while ((c = getopt(argc, argv, "crf:o:")) != -1) {
	switch(c) {
	case 'c': compile = true; break;
	case 'r': compile = false; break;
	case 'f': input = optarg; break;
	case 'o': output = optarg; break;
	default: usage(); break;
	}
    }
    
    if (compile) { // compile a file
	uint8_t buffer[12];
	uint64_t pc = 0;
	char* command = malloc(3);
	size_t command_size = 3;

	append_opcode(buffer, &pc, 0);
	append_opcode(buffer, &pc, 0);
	while(getline(&command, &command_size, stdin) != -1) {
	    if (0 == strcmp(command, "pop\n")) {
		append_opcode(buffer, &pc, 0);
	    } else if (0 == strcmp(command, "nop\n")) {
		append_opcode(buffer, &pc, 1);
	    } else if (0 == strcmp(command, "end\n")) {
		append_opcode(buffer, &pc, 2);
	    } else if (0 == strcmp(command, "add\n")) {
		append_opcode(buffer, &pc, 3);
	    } else if (0 == strcmp(command, "sub\n")) {
		append_opcode(buffer, &pc, 4);
	    } else if (0 == strcmp(command, "mul\n")) {
		append_opcode(buffer, &pc, 5);
	    } else if (0 == strcmp(command, "div\n")) {
		append_opcode(buffer, &pc, 6);
	    } else if (0 == strcmp(command, "and\n")) {
		append_opcode(buffer, &pc, 7);
	    } else if (0 == strcmp(command, "or\n")) {
		append_opcode(buffer, &pc, 8);
	    } else if (0 == strcmp(command, "xor\n")) {
		append_opcode(buffer, &pc, 9);
	    } else if (0 == strcmp(command, "not\n")) {
		append_opcode(buffer, &pc, 0xA);
	    } else if (0 == strcmp(command, "read\n")) {
		append_opcode(buffer, &pc, 0xB);
	    } else if (0 == strcmp(command, "write\n")) {
		append_opcode(buffer, &pc, 0xC);
	    } else if (0 == strcmp(command, "call\n")) {
		append_opcode(buffer, &pc, 0xD);
	    } else if (0 == strncmp(command, "push ", 5)) {
		append_opcode(buffer, &pc, 0xE);
		char arg[10];
		strcpy(arg, command+4);
		for (uint8_t i = 0; i < 10; ++i)
		    if (arg[i] == '\n') {
			arg[i] = 0;
			break;
		    }
		uint8_t arg_int = atoi(arg) & 0xf;
		append_opcode(buffer, &pc, arg_int);
	    }
	}

	for(uint8_t i = 0; i < pc; ++i) {
	    if(0 == i%2) {
		printf("%x ", (buffer[i/2] & 0xf0)>>4);
	    } else {
		printf("%x ", buffer[(i-1)/2] & 0x0f);
	    }
	}

	write_file("a.out", buffer, pc+pc%2);
    } else { // interpret a compiled file
	machine simple_machine = {
	    .program = load_file(input),
	    .data = stack_new(128),
	    .running = true,
	    .pc = 0 };
	machine_execute(&simple_machine);
    }
    
    return 0;
}
