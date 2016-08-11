#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>

char **filenames;
char **descriptions;
int file_count;
char *home_directory;

int handle_arguments(int argc, char *argv[]);
void usage();
int load_scripts();
int execute(char *program, char *arguments[]);

int main(int argc, char *argv[]) {
	int return_code;
	char *program;
	char **arguments;
	int index;
	char executable[1024];

	load_scripts();

	return_code = handle_arguments(argc, argv);
	if (return_code > 0) {
		return return_code;
	} else if (return_code < 0) {
		return 0;
	}

	arguments = (char**)malloc(sizeof(char*) * (argc + 1));
	program = strdup(getenv("SHELL"));
	arguments[0] = strdup(getenv("SHELL"));
	strcpy(executable, home_directory);
	strcat(executable, "/.shtb/");
	strcat(executable, argv[1]);
	arguments[1] = strdup(executable);
	for (index = 2; index < argc; ++index) {
		arguments[index] = strdup(argv[index]);
	}
	arguments[argc] = NULL;
	return_code = execute(program, arguments);

	for (index = 0; index < file_count; ++index) {
		free(filenames[index]);
		free(descriptions[index]);
	}
	for (index = 0; index < argc; ++index) {
		free(arguments[index]);
	}
	free(filenames);
	free(descriptions);
	free(arguments);
	free(program);

	return return_code;
}

int handle_arguments(int argc, char *argv[]) {
	int index;

	if (argc == 1) {
		usage();
		return -1;

	} else if (argc == 2) {
		if (strcmp(argv[1], "-l") == 0) {
			for (index = 0; index < file_count; ++index) {
				printf("%s\n", filenames[index]);
			}
			return -1;
		} else if (strcmp(argv[1], "-h") == 0) {
			usage();
			return -1;
		} else if (strcmp(argv[1], "-p") == 0) {
			usage();
			return 1;
		} else if (strcmp(argv[1], "-r") == 0) {
			usage();
			return 1;
		} else if (strcmp(argv[1], "-d") == 0) {
			for (index = 0; index < file_count; ++index) {
				printf("%s: %s\n--------\n", filenames[index], descriptions[index]);
			}
			return -1;
		} else {
			for (index = 0; index < file_count; ++index) {
				if (strcmp(argv[1], filenames[index]) == 0) {
					return 0;
				}
			}

			fprintf(stderr, "%s: %s\n", argv[1], "Tool not found.");
			return 2;
		}

	} else if (argc == 3) {
		if (strcmp(argv[1], "-d") == 0) {
			for (index = 0; index < file_count; ++index) {
				if (strcmp(argv[2], filenames[index]) == 0) {
					printf("%s: %s\n", filenames[index], descriptions[index]);
					return -1;
				}
			}

			fprintf(stderr, "%s: %s\n", argv[2], "Tool not found.");
			return 2;
		} else if (strcmp(argv[1], "-p") == 0) {
			FILE *source, *target;
			char copied[1024];
			char buf;

			source = fopen(argv[2], "r");
			if (source == NULL) {
				perror("fopen");
				return 4;
			}
			strcpy(copied, home_directory);
			strcat(copied, "/.shtb/");
			strcat(copied, argv[2]);
			target = fopen(copied, "w");
			if (target == NULL) {
				perror("fopen");
				return 4;
			}
			while ((buf = fgetc(source)) != EOF) {
				fputc(buf, target);
			}

			fclose(source);
			fclose(target);
			printf("%s %s\n", argv[2], "put into the toolbox.");
			return -1;
		} else if (strcmp(argv[1], "-r") == 0) {
			char removal[1024];

			strcpy(removal, home_directory);
			strcat(removal, "/.shtb/");
			strcat(removal, argv[2]);
			if (remove(removal) != 0) {
				perror("remove");
				return 3;
			} else {
				printf("%s %s\n", argv[2], "removed from the toolbox.");
				return -1;
			}
		} else {
			fprintf(stderr, "%s\n", "Invalid arguments.");
			return 1;
		}
	} else {
		fprintf(stderr, "%s\n", "Invalid arguments.");
		return 1;
	}

	return 0;
}

void usage() {
	printf("Usage:\n"
		"-l: List the contents of the toolbox.\n"
		"-d: List the contents along with the descriptions.\n"
		"-d \"toolname\": Show the description of the \"toolname\".\n"
		"-h: Show this usage help.\n");
}

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
			++file_count;
		}
	}
	if (file_count == 0) {
		return 3;
	}

	rewinddir(shtb_directory);
	filenames = (char**)malloc(sizeof(char*) * file_count);
	descriptions = (char**)malloc(sizeof(char*) * file_count);

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

int execute(char *program, char *arguments[]) {
	pid_t parent_id;
	int status;

	parent_id = fork();
	if (parent_id < 0) {
		fprintf(stderr, "%s\n", "Couldn't create a child process for tool execution.");
		return -1;
	} else if (parent_id > 0) {
		waitpid(parent_id, &status, 0);
		return status;
	} else {
		execv(program, arguments);
	}

	return -1;
}