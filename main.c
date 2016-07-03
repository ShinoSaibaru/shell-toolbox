#include <stdio.h>
#include <unistd.h>

#include "declarations.h"

char **filenames;
char **descriptions;
int file_count;

int main(int argc, char *argv[]) {
	int return_code;

	load_scripts();

	return_code = handle_arguments(argc, argv);
	if (return_code > 0) {
		return return_code;
	} else if (return_code < 0) {
		return 0;
	}

	return_code = execute(argv + 1);

	return return_code;
}