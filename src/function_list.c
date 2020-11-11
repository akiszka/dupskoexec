#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "function_list.h"

static funlist* first_function = NULL;
static funlist* current_function = NULL;
static calllist* first_call = NULL;
static calllist* current_call = NULL;

void funlist_add(uint64_t pc, char* name) {
    funlist* new = malloc(sizeof(funlist));
    new->pc = pc;
    new->name = name;
    
    if (NULL != current_function) current_function->next = new;
    else first_function = new;
    current_function = new;
}

void calllist_add(uint64_t pc, char* name) {
    calllist* new = malloc(sizeof(calllist));
    new->pc = pc;
    new->name = name;
    
    if (NULL != current_call) current_call->next = new;
    else first_call = new;
    current_call = new;
}

void calllist_fill_buffer(uint8_t* buffer) {
    for (calllist* i = first_call; i != NULL; i = i->next) {
	for (funlist* j = first_function; j != NULL; j = j->next) {
	    if (0 == strcmp(i->name, j->name)) {
		size_t index = (i->pc - i->pc%2)/2;
		if (0 == (i->pc % 2)) { // two half-bytes were left for us
		    buffer[index] = j->pc; // on even pc just fill the value
		} else { // on odd pc fill the first part into one place, then the second part into the next array index
		    buffer[index] &= 0xf0;
		    buffer[index] |= (j->pc & 0xf0) >> 4;
		    buffer[index+1] &= 0x0f;
		    buffer[index+1] |= (j->pc & 0x0f) << 4;
		}
	    }
	}
    }
}

void funlist_free() {
    funlist* i = first_function;
    while (NULL != i) {
	funlist* tmp = i;
	i = i->next;
	free(tmp->name);
	free(tmp);
    }
}

void calllist_free() {
    funlist* i = first_call;
    while (NULL != i) {
	funlist* tmp = i;
	i = i->next;
	free(tmp->name);
	free(tmp);
    }
}
