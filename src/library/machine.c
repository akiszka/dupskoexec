#include <stdint.h>
#include <stdlib.h>

#include "machine.h"
#include "stack.h"
#include "ops.h"

machine* machine_new(uint8_t** program, const size_t data_size, const size_t return_size) {
    machine* result = malloc(sizeof(machine));

    result->program = program;
    result->data = stack_new(data_size);
    result->return_pointer = stack_new(return_size);
    result->running = true;
    result->pc = 0;

    return result;
}

void machine_delete(machine* obj) {
    free(*obj->program);
    stack_delete(&(obj->data));
    stack_delete(&(obj->return_pointer));
    free(obj);
}

void machine_execute(machine* obj) {    
    while(machine_load_data_next(obj));
    while(machine_execute_next(obj));
}

void machine_run(uint8_t** program, size_t data_size, size_t return_pointer_size) {
    machine* machine = machine_new(program, data_size, return_pointer_size);
    machine_execute(machine);
    machine_delete(machine);
}

bool machine_load_data_next(machine* obj) { // returns true if we're to keep loading data
    uint8_t piece_of_data = (*obj->program)[obj->pc/2];
    if (piece_of_data == 0) {
	obj->pc += 2;
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
    uint8_t opcode;
    if(0 == obj->pc % 2)
	opcode = ((*obj->program)[obj->pc/2] & 0xf0) >> 4;
    else
	opcode = (*obj->program)[(obj->pc-1)/2] & 0x0f;
    return opcode;
}

void machine_set_pc(machine* obj, uint64_t pc) {
    obj->pc = pc;
}

uint64_t machine_get_pc(machine* obj) {
    return obj->pc;
}
