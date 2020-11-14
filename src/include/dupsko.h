#pragma once

#include <stdint.h>

void machine_run(uint8_t* program, size_t data_size, size_t return_pointer_size);
void compile(char* input_fname, char* output_fname);
void usage(); // NOT defined by the library, this has to be provided by the user
