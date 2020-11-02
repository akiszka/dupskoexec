#pragma once

struct STACK_t {
    uint8_t sp;
    uint8_t size;
    uint8_t* stack;
};
typedef struct STACK_t stack;

uint8_t stack_push(stack* obj, uint8_t new_item);
uint8_t stack_pop(stack *obj);
