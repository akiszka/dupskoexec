#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ops.h"
#include "stack.h"
#include "machine.h"
#include "compiler.h"

uint8_t* load_executable_file(char* filename) {
    FILE* f;
    uint8_t* code = NULL;
    struct stat st;
    
    if ((f = fopen(filename, "r"))) {
	fstat(fileno(f), &st);
	code = malloc(st.st_size);
	fread(code, 1, st.st_size, f);
	fclose(f);
    } else {
	printf("ERROR: invalid file %s\n", filename);
	usage();
    }

    return code;
}

int main(int argc, char** argv) {
    char c;
    bool do_compile = true;
    char* input = NULL;
    char* output = NULL;

    while ((c = getopt(argc, argv, "cif:o:")) != -1) {
	switch(c) {
	case 'c': do_compile = true; break;
	case 'i': do_compile = false; break;
	case 'f': input = optarg; break;
	case 'o': output = optarg; break;
	default: usage(); break;
	}
    }
    
    if (do_compile) { // compile a file
	if (input == NULL)  input = "a.in";
	if (output == NULL) output = "a.out";
	compile(input, output);
    } else { // interpret a compiled file
	if (input == NULL) input = "a.out";
	machine simple_machine = {
	    .program = load_executable_file(input),
	    .data = stack_new(128),
	    .return_pointer = stack_new(128),
	    .running = true,
	    .pc = 0 };
	machine_execute(&simple_machine);
	machine_delete(&simple_machine);
    }
    
    return 0;
}
