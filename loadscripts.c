#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "declarations.h"

char **filenames;
char **descriptions;
int files_count;
char *home_directory;

int load_scripts() {
	DIR *shtb_directory;
	struct dirent *shtb_content;
	int index;
	FILE *test_file;
	char description_string[16384];
	int file_size;
	char *beginning_pointer, *description_pointer, *second_description_pointer;
	char helper_path[16384];

	home_directory = strdup(getenv("HOME"));
	strcpy(helper_path, home_directory);
	strcat(helper_path, "/.shtb");
	shtb_directory = opendir(helper_path);
	if (shtb_directory == NULL) {
		perror("opendir");
		return 1;
	}

	while (shtb_content = readdir(shtb_directory)) {
		if (shtb_content->d_type == DT_REG) {
			++files_count;
		}
	}
	if (files_count == 0) {
		return 3;
	}

	rewinddir(shtb_directory);
	filenames = (char**)malloc(sizeof(char*) * files_count);
	descriptions = (char**)malloc(sizeof(char*) * files_count);

	for (index = 0; shtb_content = readdir(shtb_directory); ) {
		if (shtb_content->d_type == DT_REG) {
			filenames[index] = strdup(shtb_content->d_name);
			strcpy(helper_path, home_directory);
			strcat(helper_path, "/.shtb/");
			strcat(helper_path, filenames[index]);
			test_file = fopen(helper_path, "r");
			if (test_file == NULL) {
				perror("fopen");
				return 2;
			}

			fseek(test_file, 0, SEEK_END);
			file_size = ftell(test_file);
			rewind(test_file);
			if (fread(description_string, 1, file_size, test_file) == 0) {
				perror("fread");
				return 4;
			}
			description_string[file_size] = '\0';

			beginning_pointer = description_pointer = strstr(description_string, "#description");
			if (description_pointer == NULL) {
				descriptions[index] = strdup("No description.");
			} else {
				second_description_pointer = description_pointer + 12;
				if (*second_description_pointer == '\0') {
					descriptions[index] = strdup("No description.");
				} else {
					description_pointer = strstr(description_pointer, "#enddescription");
					if (description_pointer == NULL) {
						second_description_pointer = strstr(second_description_pointer, "\n");
						if (second_description_pointer != NULL) {
							*second_description_pointer = '\0';
						}
						descriptions[index] = strdup(beginning_pointer + 12);
					} else {
						*description_pointer = '\0';
						descriptions[index] = strdup(second_description_pointer);
					}
				}
			}

			fclose(test_file);
			++index;
		}
	}

	closedir(shtb_directory);
}