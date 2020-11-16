#pragma once

#include <stdint.h>

struct FUNCTION_CALL_LIST_t {
    uint8_t pc;
    char* name;
    struct FUNCTION_CALL_LIST_t* prev;
};

typedef struct FUNCTION_CALL_LIST_t funlist;
typedef struct FUNCTION_CALL_LIST_t callstack;

void funlist_add(uint64_t pc, char* name);
void callstack_add(uint64_t pc, char* name);

int call_try_fill(uint8_t** buffer);
void call_fill_buffer(uint8_t** buffer);

void callstack_pop();
void callstack_free();
void funlist_free();
