#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "function_list.h"

static funlist* function_list = NULL;
static callstack* call_stack = NULL;

void funlist_add(uint64_t pc, char* name) {
    funlist* new = malloc(sizeof(funlist));
    new->pc = pc;
    new->name = name;
    new->prev = function_list;
    function_list = new;
}

void callstack_add(uint64_t pc, char* name) {
    callstack* new = malloc(sizeof(callstack));
    new->pc = pc;
    new->name = name;
    new->prev = call_stack;
    call_stack = new;
}

int call_try_fill(uint8_t** buffer) {
    if (NULL == call_stack) return -1; // there isn't anything to fill anymore
    
    for (funlist* i = function_list; i != NULL; i = i->prev) {
	if (0 == strcmp(i->name, call_stack->name)) {
	    uint64_t index = (call_stack->pc - call_stack->pc%2)/2;

	    if (0 == (call_stack->pc % 2)) { // two half-bytes were left for us
		(*buffer)[index] = i->pc; // on even pc just fill the value
	    } else { // on odd pc fill the first part, then the second part into the next array index
		(*buffer)[index] &= 0xf0;
		(*buffer)[index] |= (i->pc & 0xf0) >> 4;
		(*buffer)[index+1] &= 0x0f;
		(*buffer)[index+1] |= (i->pc & 0x0f) << 4;
	    }
	    
	    callstack_pop();
	    return 0;
	}
    }
    return -2; // not found
}

void call_fill_buffer(uint8_t** buffer) {
    while (call_try_fill(buffer) == 0);
}

void callstack_pop() {
    callstack* tmp = call_stack;
    call_stack = tmp->prev;
    free(tmp->name);
    free(tmp);
}

void callstack_free() {
    while (NULL != call_stack) {
	callstack_pop();
    }
}

void funlist_free() {
    while (NULL != function_list) {
	funlist* tmp = function_list;
	function_list = tmp->prev;
	free(tmp->name);
	free(tmp);
    }
}

