#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "declarations.h"

int load_scripts() {
	DIR *shtbdir;
	struct dirent *shtbcontent;
	int index;
	FILE *test_binary;
	char *test_buffer;
	char test_string[16384];

	shtbdir = opendir("~/.shtb");
	if (shtbdir == NULL) {
		perror("opendir");
		return 1;
	}

	while (shtbcontent = readdir("~/.shtb")) {
		if (shtbcontent->d_type == DT_REG) {
			++files_count;
		}
	}

	if (files_count == 0) {
		return 3;
	}

	rewinddir(shtbdir);
	filenames = (char**)malloc(files_count * sizeof(char*));
	descriptions = (char**)malloc(files_count * sizeof(char*));

	for (index = 0; shtbcontent = readdir("~/.shtb"); ++index) {
		filenames[index] = strdup(shtbcontent->d_name);
		test_binary = fopen(filenames[index]);
		if (test_binary == NULL) {
			perror("fopen");
			return 2;
		}

		test_buffer = fread(test_buffer, 128, test_binary);
		if (memchr(test_buffer, (char)0, strlen(test_buffer)) == NULL) {
			rewind(test_binary);
			test_string = fread(test_buffer, sizeof(test_string) - 1, test_binary);

		} else {
			descriptions[index] = strdup("Binary file.");
		}

		fclose(test_binary)
	}
}