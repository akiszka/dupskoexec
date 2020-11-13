#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dupsko.h>

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
    char* input = NULL;
 
    while ((c = getopt(argc, argv, "cif:o:")) != -1) {
	switch(c) {
	case 'f': input = optarg; break;
	default: usage(); break;
	}
    }
    
    if (input == NULL) input = "a.out";
    machine_run(load_executable_file(input), 128, 128);
    
    return 0;
}
