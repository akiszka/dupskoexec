#include <stdint.h>
#include <stdlib.h>

#include "machine.h"
#include "stack.h"
#include "ops.h"

void machine_delete(machine* obj) {
    free(obj->program);
    stack_delete(&(obj->data));
    stack_delete(&(obj->return_pointer));
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
    opcodes[machine_get_opcode(obj)](obj);
    ++obj->pc;
    return obj->running;
}

uint8_t machine_get_opcode(machine* obj) {    
    if(0 == obj->pc % 2)
	return (obj->program[obj->pc/2] & 0xf0) >> 4;
    else
	return obj->program[(obj->pc-1)/2] & 0x0f;
}
