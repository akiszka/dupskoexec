#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef void machine;

void machine_run(uint8_t** program, size_t data_size, size_t return_pointer_size);
machine* machine_new(uint8_t** program, const size_t data_size, const size_t return_size);
void machine_delete(machine* obj);
bool machine_execute_next(machine* obj);
void machine_set_pc(machine* obj, uint64_t pc);
uint64_t machine_get_pc(machine* obj);
    
void compile(char* input_fname, char* output_fname);
int compiler_generate_command(uint8_t** buffer, size_t* buffer_size, size_t* pc, char* command);

void usage(); // NOT defined by the library, this has to be provided by the user
