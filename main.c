#include <stdio.h>
#include <unistd.h>
#include <getenv.h>

#include "declarations.h"

char **filenames;
char **descriptions;
int file_count;
char *home_directory;

int main(int argc, char *argv[]) {
	int return_code;
	char **arguments;
	char buffer[1024] = "~/.shtb/";
	int index;

	load_scripts();

	return_code = handle_arguments(argc, argv);
	if (return_code > 0) {
		return return_code;
	} else if (return_code < 0) {
		return 0;
	}

	arguments = malloc(sizeof(char*) * (argc + 1));
	for (index = 0; index < file_count; ++index) {
		if (strcmp(argv[1], filenames[index]) == 0) {
			break;
		}
	}
	arguments[0] = strdup(getenv("SHELL"));
	arguments[1] = strdup(getenv("SHELL"));

	for (index = 2; index < argc; ++index) {
		arguments[index] = strdup(argv[index]);
	}
	arguments[argc] = NULL;
	return_code = execute(arguments);

	for (index = 0; index < file_count; ++index) {
		free(filenames[index]);
		free(descriptions[index]);
	}
	for (index = 0; index <= argc; ++index) {
		free(arguments[index]);
	}
	free(filenames);
	free(descriptions);
	free(arguments);

	return return_code;
}