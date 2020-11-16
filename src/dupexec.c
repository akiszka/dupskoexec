#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dupsko.h>

char* myname;

void usage() {
    printf("Usage: %s -f <input>\n", myname);
    exit(1);
}

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
    char* input = "a.out";

    myname = argv[0];
    
    while ((c = getopt(argc, argv, "f:")) != -1) {
	switch(c) {
	case 'f': input = optarg; break;
	default: usage(); break;
	}
    }
    
    uint8_t** code =  malloc(sizeof(uint8_t*));
    *code = load_executable_file(input);

    machine_run(code, 128, 128);

    free(code);
    return 0;
}
