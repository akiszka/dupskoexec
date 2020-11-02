#pragma once

#include "machine.h"

typedef void (*opcode_t)(machine*); // opcode function

void op_pop(machine* obj);
void op_push(machine* obj);
void op_nop(machine* obj);
void op_stop(machine* obj);
void op_add(machine* obj);
void op_sub(machine* obj);
void op_mul(machine* obj);
void op_div(machine* obj);
void op_and(machine* obj);
void op_or(machine* obj);
void op_xor(machine* obj);
void op_not(machine* obj);
void op_read(machine* obj);
void op_write(machine* obj);
void op_call(machine* obj);

const static opcode_t opcodes[16] = {
    op_pop,
    op_nop,
    op_stop,
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_and,
    op_or,
    op_xor,
    op_not,
    op_read,
    op_write,
    op_call,
    op_push,
    op_nop // reserved for the future
};
