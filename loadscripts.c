#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "declarations.h"

char **filenames;
char **descriptions;
int files_count;

int load_scripts() {
	DIR *shtbdir;
	struct dirent *shtb_content;
	int index;
	FILE *test_binary;
	char *test_buffer;
	char test_string[16384];
	char *beginning_pointer, *description_pointer, *second_description_pointer;

	shtb_directory = opendir("~/.shtb");
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
	filenames = (char**)malloc(files_count * sizeof(char*));
	descriptions = (char**)malloc(files_count * sizeof(char*));

	for (index = 0; shtb_content = readdir(shtb_directory); ++index) {
		filenames[index] = strdup(shtb_content->d_name);
		test_binary = fopen(filenames[index]);
		if (test_binary == NULL) {
			perror("fopen");
			return 2;
		}

		test_buffer = fread(test_buffer, 128, test_binary);
		if (memchr(test_buffer, (char)0, strlen(test_buffer)) == NULL) {
			rewind(test_binary);
			test_string = fread(test_buffer, sizeof(test_string) - 1, test_binary);
			beginning_description = description_pointer = strstr(test_string, "\n#description ");
			if (description_pointer == NULL) {
				descriptions[index] = strdup("No description.");
			} else {
				second_description_pointer = description_pointer + 14;
				description_pointer = strstr(description_pointer, "\n#enddescription");
				if (description_pointer == NULL) {
					second_description_pointer = strstr(second_description_pointer, "\n");
					if (second_description_pointer == NULL) {
						descriptions[index] = strdup(beginning_description);
					} else {
						second_description_pointer = '\0';
						descriptions[index] = strdup(beginning_description);
					}
				} else {
					description_pointer = '\0';
					descriptions[index] = strdup(second_description_pointer);
				}
			}
		} else {
			descriptions[index] = strdup("Binary file.");
		}

		fclose(test_binary)
	}

	closedir(shtb_directory);
}