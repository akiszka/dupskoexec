#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "ops.h"
#include "stack.h"

void op_pop(machine* obj) {
    stack_pop(&(obj->data));
}

void op_push(machine* obj) {
    uint8_t new;
    ++obj->pc;
    if(0 == obj->pc % 2) {
	new = (obj->program[obj->pc/2] & 0xf0) >> 4;
    } else {
	new = obj->program[(obj->pc-1)/2] & 0x0f;
    }
    stack_push(&(obj->data), new);
}

void op_nop(machine* obj) {
    return;
}

void op_stop(machine* obj) {
    obj->running = false;
}

void op_add(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) + stack_pop(&(obj->data));
    stack_push(&(obj->data), result);
}

void op_sub(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) - stack_pop(&(obj->data));
    stack_push(&(obj->data), result);
}

void op_mul(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) * stack_pop(&(obj->data));
    stack_push(&(obj->data), result);
}

void op_div(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) / stack_pop(&(obj->data));
    stack_push(&(obj->data), result);
}

void op_and(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) & stack_pop(&(obj->data));
    stack_push(&(obj->data), result);
}

void op_or(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) | stack_pop(&(obj->data));
    stack_push(&(obj->data), result);
}

void op_xor(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) ^ stack_pop(&(obj->data));
    stack_push(&(obj->data), result);
}

void op_not(machine* obj) {
    uint8_t new_value = ~stack_pop(&(obj->data));
    stack_push(&(obj->data), new_value);
}

void op_read(machine* obj) {
    return;
}

void op_write(machine* obj) {
    char to_display = stack_pop(&(obj->data));
    printf("%c", to_display);
}

void op_call(machine* obj) {
    return;
}
