#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <dupsko.h>

char* myname;

void usage() {
    printf("Usage: %s -f <input>\n", myname);
    exit(1);
}

void compile_jit(char* input_fname) {
    size_t pc = 0, command_size = 3, buffer_size = 3;
    uint8_t* buffer = malloc(buffer_size);
    char* command = malloc(command_size);
    
    FILE* input = fopen(input_fname, "r");
    if (NULL == input) {
	printf("ERROR: invalid file %s\n", input_fname);
	usage();
    }

    machine* my_machine = machine_new(&buffer, 128, 128);
    
    while(getline(&command, &command_size, input) != -1) {
	pc = 0;
	machine_set_pc(my_machine, 0);
	
	int result = compiler_generate_command(&buffer, &buffer_size, &pc, command);

	if (result == -1) {
	    printf("ERROR! Function is not defined.\n");
	    exit(2);
	}

	while (machine_get_pc(my_machine) != pc) {
	    if (machine_execute_next(my_machine) == false) break;
	}
    }
    
    machine_delete(my_machine);
    
    free(input);
    free(command);
}

int main(int argc, char** argv) {
    char c;
    char* input = "a.in";

    myname = argv[0];

    while ((c = getopt(argc, argv, "f:")) != -1) {
	switch(c) {
	case 'f': input = optarg; break;
	default: usage(); break;
	}
    }

    compile_jit(input);
    
    return 0;
}
