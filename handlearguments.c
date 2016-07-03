#include <string.h>
#include <stdio.h>

#include "declarations.h"

char **filenames;
char **descriptions;
int file_count;

int handle_arguments(int argc, char *argv[]) {
	int index;

	if (argc == 1) {
		usage();
		return 1;

	} else if (argc == 2) {
		if (strcmp(argv[1], "-l") == 0) {
			for (index = 0; index < file_count; ++index) {
				printf("%s\n", filenames[index]);
			}
			return -1;
		} else if (strcmp(argv[1], "-h") == 0) {
			usage();
			return -1;
		} else {
			return 1;
		}

	} else if (argc == 3) {
		if (strcmp(argv[1], "-d") == 0) {
			for (index = 0; index < file_count; ++index) {
				if (strcmp(argv[2], filenames[index])) {
					printf("%s: %s\n", filenames[index], descriptions[index]);
					return -2;
				}
			}

			fprintf(stderr, "%s: %s\n", argv[2], "Tool not found.");
			return 2;
		} else {
			return 1;
		}
	}
}