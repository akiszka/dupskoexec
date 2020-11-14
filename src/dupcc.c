#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dupsko.h>

char* myname;

void usage() {
    printf("Usage: %s -f <input> -o <output>\n", myname);
    exit(1);
}

int main(int argc, char** argv) {
    char c;
    char* input = NULL;
    char* output = NULL;

    myname = argv[0];
    
    while ((c = getopt(argc, argv, "f:o:")) != -1) {
	switch(c) {
	case 'f': input = optarg; break;
	case 'o': output = optarg; break;
	default: usage(); break;
	}
    }

    if (input == NULL)  input = "a.in";
    if (output == NULL) output = "a.out";
    
    compile(input, output);
    
    return 0;
}
