#pragma once

#include <stdint.h>

struct FUNCTION_LIST_t {
    uint8_t pc;
    char* name;
    struct FUNCTION_LIST_t* next;
};
typedef struct FUNCTION_LIST_t funlist;
typedef struct FUNCTION_LIST_t calllist;

void funlist_add(uint64_t pc, char* name);
void calllist_add(uint64_t pc, char* name);
void calllist_fill_buffer(uint8_t* buffer);
void funlist_free();
void calllist_free();
