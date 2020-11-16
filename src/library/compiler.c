#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "compiler.h"
#include "function_list.h"

void usage(); // defined in the main program

void compile(char* input_fname, char* output_fname) {
    size_t pc = 0, command_size = 3, buffer_size = 3;
    uint8_t* buffer = malloc(buffer_size);
    char* command = malloc(command_size);

    FILE* input = fopen(input_fname, "r");
    if (NULL == input) {
	printf("ERROR: invalid file %s\n", input_fname);
	usage();
    }

    append_opcode(&buffer, &buffer_size, &pc, 0);
    append_opcode(&buffer, &buffer_size, &pc, 0);
    
    while(getline(&command, &command_size, input) != -1) {
	compiler_generate_command(&buffer, &buffer_size, &pc, command);
    }

    call_fill_buffer(&buffer);
    funlist_free();
    callstack_free();
    
    write_file(output_fname, buffer, (pc+pc%2)/2);
    
    fclose(input);
    free(buffer);
    free(command);
}

int compiler_generate_command(uint8_t** buffer, size_t* buffer_size, size_t* pc, char* command) {
    int result = 0;
    
    if (CHECK_COMMAND("pop\n"))         INSERT_OP(0);
    else if (CHECK_COMMAND("nop\n"))    INSERT_OP(1);
    else if (CHECK_COMMAND("end\n"))    INSERT_OP(2);
    else if (CHECK_COMMAND("add\n"))    INSERT_OP(3);
    else if (CHECK_COMMAND("sub\n"))    INSERT_OP(4);
    else if (CHECK_COMMAND("mul\n"))    INSERT_OP(5);
    else if (CHECK_COMMAND("div\n"))    INSERT_OP(6);
    else if (CHECK_COMMAND("and\n"))    INSERT_OP(7);
    else if (CHECK_COMMAND("or\n"))     INSERT_OP(8);
    else if (CHECK_COMMAND("xor\n"))    INSERT_OP(9);
    else if (CHECK_COMMAND("not\n"))    INSERT_OP(0xA);
    else if (CHECK_COMMAND("read\n"))   INSERT_OP(0xB);
    else if (CHECK_COMMAND("write\n"))  INSERT_OP(0xC);
    else if (CHECK_COMMAND("return\n")) INSERT_OP(0xF);
    else if (CHECK_COMMAND_ARG("push ", 5)) {
	char arg[10];
	strncpy(arg, command+5, 10);
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
    } else if (CHECK_COMMAND_ARG("call ", 5)) {
	char* arg = malloc(20);
	strncpy(arg, command+5, 20);
	for (uint8_t i = 0; i < 20; ++i)
	    if (arg[i] == '\n') {
		arg[i] = 0;
		break;
	    }
	INSERT_OP(0xD);
	callstack_add(*pc, arg);
	INSERT_OP(0); // placeholder ops, will be replaced later by calllist_fill_buffer()
	INSERT_OP(0);
	// there musn't be a free here
	if (call_try_fill(buffer) == -2) result = -1; // there isn't a function defined yet... we return a special value in case this is a JIT compiler and a function won't be declared later
    } else if (CHECK_COMMAND_ARG("fun ", 4)) {
	char* arg = malloc(20);
	strncpy(arg, command+4, 20);
	for (uint8_t i = 0; i < 20; ++i)
	    if (arg[i] == '\n') {
		arg[i] = 0;
		break;
	    }
	funlist_add(*(pc)-1, arg);
	// there musn't be a free here
    } else if (CHECK_COMMAND_ARG("times ", 6)) {
	char* arg1_amount = malloc(20);
	char* arg2_command = malloc(20);
	uint8_t amount = 0, arg2_starts_after;

	// get the amount of commands to generate
	strncpy(arg1_amount, command+6, 20);
	compiler_break_str_after(&arg1_amount, ' ', strlen(arg1_amount));
	amount = atoi(arg1_amount);
	arg2_starts_after = 6 + strlen(arg1_amount)+1; // arg2 position = the command + arg1

	// get the command to generate
	strncpy(arg2_command, command+arg2_starts_after, 20);

	// generate the given amount
	while (amount != 0) {
	    compiler_generate_command(buffer, buffer_size, pc, arg2_command);
	    --amount;
	}
	
	free(arg1_amount);
	free(arg2_command);
    }

    return result;
}

void compiler_break_str_after(char** string, const char what_to_look_for, const uint8_t amount) {
    for (uint8_t i = 0; i < amount; ++i) {
	if ((*string)[i] == what_to_look_for) {
	    (*string)[i] = '\0';
	    return;
	}
    }
}

void append_opcode(uint8_t** buffer, size_t* buffer_size, size_t* counter, uint8_t opcode) {
    opcode &= 0x0f; // ensure that the opcode is only 4 bits
    size_t index = (*counter - (*counter)%2)/2; // get the array index
    
    if (*buffer_size <= index+5) { // if we have less than 5 spaces in the buffer left, allocate some more, TODO: adaptive resizing
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
