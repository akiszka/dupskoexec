#pragma once

#include <stdbool.h>

#include "stack.h"

struct MACHINE_t {
    uint64_t pc;
    stack data;
    stack return_pointer;
    bool running;
    uint8_t* program;
};
typedef struct MACHINE_t machine;

void machine_delete(machine* obj);
void machine_execute(machine* obj);
bool machine_execute_next(machine* obj);
bool machine_load_data_next(machine* obj);
uint8_t machine_get_opcode(machine* obj);