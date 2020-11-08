#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "compiler.h"

void usage() {
    printf("usage: dupsko -f <input> -o <output> -ic\n-i -> interpret\n-c -> compile\n");
    exit(1);
}

void write_file(char* filename, uint8_t* buffer, size_t len) {
    FILE* f;

    if ((f = fopen(filename, "wb"))) {
	fwrite(buffer, sizeof(uint8_t), len, f);
	fclose(f);
    } else {
	printf("ERROR: invalid file %s\n", filename);
	usage();
    }
}

void append_opcode(uint8_t** buffer, size_t* buffer_size, uint64_t* counter, uint8_t opcode) {
    opcode = opcode & 0x0f; // ensure that the opcode os only 1/2 byte
    
    int index = (*counter - (*counter)%2)/2; // get the index
    if (*buffer_size <= index+5) { // if we have less than 5 spaces in the buffer left, allocate some more
	uint8_t* buffer_tmp = realloc(*buffer, (*buffer_size)+10);

	if (NULL != buffer_tmp) {
	    *buffer_size += 10;
	    *buffer = buffer_tmp;
	} else {
	    printf("Memory error!\n");
	    exit(1);
	}
    }
    
    if (0 == (*counter % 2)) { // on even PC get the higher 4 bits
	(*buffer)[index] = 0;
	(*buffer)[index] = opcode<<4;
    } else { // on odd PC get the lower 4 bits
	(*buffer)[index] |= opcode;
    }
    ++*counter; // increase the PC
}

void compile(char* input_fname, char* output_fname) {
    size_t pc = 0, command_size = 3, buffer_size = 3;
    uint8_t* buffer = malloc(buffer_size);
    char* command = malloc(command_size);

    FILE* input = fopen(input_fname, "r");
    if (NULL == input) {
	printf("ERROR: invalid file %s\n", input_fname);
	usage();
    }
    
    INSERT_OP(0);
    INSERT_OP(0);
    while(getline(&command, &command_size, input) != -1) {
	if (CHECK_COMMAND("pop\n"))        INSERT_OP(0);
	else if (CHECK_COMMAND("nop\n"))   INSERT_OP(1);
	else if (CHECK_COMMAND("end\n"))   INSERT_OP(2);
	else if (CHECK_COMMAND("add\n"))   INSERT_OP(3);
	else if (CHECK_COMMAND("sub\n"))   INSERT_OP(4);
	else if (CHECK_COMMAND("mul\n"))   INSERT_OP(5);
	else if (CHECK_COMMAND("div\n"))   INSERT_OP(6);
	else if (CHECK_COMMAND("and\n"))   INSERT_OP(7);
	else if (CHECK_COMMAND("or\n"))    INSERT_OP(8);
	else if (CHECK_COMMAND("xor\n"))   INSERT_OP(9);
	else if (CHECK_COMMAND("not\n"))   INSERT_OP(0xA);
	else if (CHECK_COMMAND("read\n"))  INSERT_OP(0xB);
	else if (CHECK_COMMAND("write\n")) INSERT_OP(0xC);
	else if (CHECK_COMMAND("call\n"))  INSERT_OP(0xD);
	else if (CHECK_COMMAND_ARG("push ", 5)) {
	    char arg[10];
	    strcpy(arg, command+4);
	    for (uint8_t i = 0; i < 10; ++i)
		if (arg[i] == '\n') {
		    arg[i] = 0;
		    break;
		}
	    uint8_t arg_int = atoi(arg);
	    INSERT_OP(0xE);
	    INSERT_OP((arg_int & 0xf0) >> 4);
	    INSERT_OP(arg_int & 0x0f);
	} else if (CHECK_COMMAND_ARG("pushc ", 6)) {
	    uint8_t arg = *(command+6);
	    INSERT_OP(0xE);
	    INSERT_OP((arg & 0xf0) >> 4);
	    INSERT_OP(arg & 0x0f);
	} else if (CHECK_COMMAND_ARG("pushs ", 6)) {
	    uint8_t length = 6;
	    while ('\n' != *(command+length)) ++length;

	    for (uint8_t i = length-1; i >= 6; --i) {
		uint8_t current_char = *(command+i);
		INSERT_OP(0xE);
		INSERT_OP((current_char & 0xf0) >> 4);
		INSERT_OP(current_char & 0x0f);
	    }
	}
    }

    write_file(output_fname, buffer, (pc+pc%2)/2);
    free(buffer);
    free(command);
}
