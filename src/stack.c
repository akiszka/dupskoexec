#include <stdint.h>
#include <stdlib.h>

#include "stack.h"

stack stack_new(uint8_t size) {
    stack stack;
    stack.sp = 0;
    stack.size = size;
    stack.stack = malloc(size * sizeof(uint8_t));
    return stack;
}

void stack_delete(stack* obj) {
    free(obj->stack);
}

uint8_t stack_push(stack* obj, uint8_t new_item) {
    obj->stack[obj->sp++] = new_item;
    return obj->sp;
}

uint8_t stack_pop(stack *obj) {
    return obj->stack[--(obj->sp)];
}
