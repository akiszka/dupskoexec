#pragma once

#include <stdint.h>
#include <string.h>

#define CHECK_COMMAND(name) (0 == strcmp(command, (name)))
#define CHECK_COMMAND_ARG(name, chars) (0 == strncmp(command, (name), (chars)))
#define INSERT_OP(op) append_opcode(buffer, buffer_size, pc, (op))

void compile(char* input_fname, char* output_fname);
void compiler_generate_command(uint8_t** buffer, size_t* buffer_size, size_t* pc, char* command);
void compiler_break_str_after(char** string, const char what_to_look_for, const uint8_t amount);
void write_file(char* filename, uint8_t* buffer, size_t len);
void append_opcode(uint8_t** buffer, size_t* buffer_size, size_t* counter, uint8_t opcode);
