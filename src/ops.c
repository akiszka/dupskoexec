#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "ops.h"
#include "stack.h"

#define PUSH_DATA(x)   stack_push(&(obj->data), (x))
#define PUSH_RETURN(x) stack_push(&(obj->return_pointer), (x))
#define POP_RETURN()   stack_pop(&(obj->return_pointer))

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
    (void)obj;
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
    (void)obj;
    return;
}

void op_write(machine* obj) {
    char to_display = stack_pop(&(obj->data));
    printf("%c", to_display);
}

void op_call(machine* obj) {
    uint8_t new_pc = 0;
    ++obj->pc; new_pc |= machine_get_opcode(obj) << 4;
    ++obj->pc; new_pc |= machine_get_opcode(obj);

    printf("CALL: old %d new %d\n", obj->pc, new_pc);
    PUSH_RETURN(obj->pc);
    obj->pc = new_pc;
}

void op_ret(machine* obj) {
    printf("RET: old %d ", obj->pc);
    obj->pc = POP_RETURN();
    printf(" new %d\n", obj->pc);
}
