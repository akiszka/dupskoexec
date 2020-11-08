#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "ops.h"
#include "stack.h"

#define PUSH_DATA(x)   stack_push(&(obj->data), (x))
#define PUSH_RETURN(x) stack_push(&(obj->return_pointer), (x))

void op_pop(machine* obj) {
    stack_pop(&(obj->data));
}

void op_push(machine* obj) {
    uint8_t piece1, piece2;
    ++obj->pc; piece1 = machine_get_opcode(obj);
    ++obj->pc; piece2 = machine_get_opcode(obj);
    PUSH_DATA((piece1 << 4) + piece2);
}

void op_nop(machine* obj) {
    return;
}

void op_stop(machine* obj) {
    obj->running = false;
}

void op_add(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) + stack_pop(&(obj->data));
    PUSH_DATA(result);
}

void op_sub(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) - stack_pop(&(obj->data));
    PUSH_DATA(result);
}

void op_mul(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) * stack_pop(&(obj->data));
    PUSH_DATA(result);
}

void op_div(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) / stack_pop(&(obj->data));
    PUSH_DATA(result);
}

void op_and(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) & stack_pop(&(obj->data));
    PUSH_DATA(result);
}

void op_or(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) | stack_pop(&(obj->data));
    PUSH_DATA(result);
}

void op_xor(machine* obj) {
    uint8_t result = stack_pop(&(obj->data)) ^ stack_pop(&(obj->data));
    PUSH_DATA(result);
}

void op_not(machine* obj) {
    uint8_t new_value = ~stack_pop(&(obj->data));
    PUSH_DATA(new_value);
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
