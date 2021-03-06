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

	return_code = load_scripts();
	if (return_code != 0) {
		return return_code;
	}

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
	free(home_directory);

	return return_code;
}

int handle_arguments(int argc, char *argv[]) {
	int index;

	if (argc == 1) {
		usage();
		return -1;
	} else if (argc == 2) {
		if (strcmp(argv[1], "-l") == 0) {
			if (file_count == 0) {
				fprintf(stderr, "%s\n", "No tools in the toolbox!");
				return 5;
			}
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
			if (file_count == 0) {
				fprintf(stderr, "%s\n", "No tools in the toolbox!");
				return 5;
			}
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

	} else if (argc >= 3) {
		char buffer[1024];
		index = 1;
		if (strcmp(argv[1], "-d") == 0) {
			if (file_count == 0) {
				fprintf(stderr, "%s\n", "No tools in the toolbox!");
				return 5;
			}
			while (++index != argc) {
				int index1;
				for (index1 = 0; index1 < file_count; ++index1) {
					if (strcmp(argv[index], filenames[index1]) == 0) {
						printf("%s: %s\n--------\n", filenames[index1], descriptions[index1]);
						break;
					}
				}
				if (index1 == file_count) {
					fprintf(stderr, "%s: %s\n", argv[index], "Tool not found.");
					return 2;
				}
			}

			return -1;
		} else if (strcmp(argv[1], "-p") == 0) {
			FILE *source, *target;
			char buf;
			char buf1[2] = {'\0', '\0'};
			while (++index != argc) {
				strcpy(buffer, home_directory);
				strcat(buffer, "/.shtb/");
				strcat(buffer, argv[index]);
				if (access(buffer, F_OK) == 0) {
					printf("%s\n", "The file exists already. Overwrite? [Y|y|N|n]");
					fgets(buf1, sizeof(buf1), stdin);
					if (buf1[1] != '\n') {
						int ch;
						while (((ch = getchar()) != '\n') && (ch != EOF));
					}
					buf1[1] = '\0';
					if (buf1[0] != 'y' && buf1[0] != 'Y') {
						continue;
					}
				}
				source = fopen(argv[index], "r");
				if (source == NULL) {
					fprintf(stderr, "%s %s:\n", "Error with ", argv[index]);
					perror("fopen");
					return 4;
				}
				target = fopen(buffer, "w");
				if (target == NULL) {
					fprintf(stderr, "%s %s:\n", "Error with ", argv[index]);
					perror("fopen");
					return 4;
				}
				while ((buf = fgetc(source)) != EOF) {
					fputc(buf, target);
				}

				fclose(source);
				fclose(target);
				printf("%s %s\n", argv[index], "put into the toolbox.");
			}

			return -1;
		} else if (strcmp(argv[1], "-r") == 0) {
			while (++index != argc) {
				strcpy(buffer, home_directory);
				strcat(buffer, "/.shtb/");
				strcat(buffer, argv[index]);
				if (remove(buffer) != 0) {
					fprintf(stderr, "%s %s:\n", "Error with ", buffer);
					perror("remove");
					return 3;
				} else {
					printf("%s %s\n", argv[index], "removed from the toolbox.");
				}
			}

			return -1;
		}
	}

	fprintf(stderr, "%s\n", "Invalid arguments.");
	return 1;
}

void usage() {
	printf("Usage:\n"
		"-l: List the contents of the toolbox.\n"
		"-d: List the contents along with the descriptions.\n"
		"-d \"toolname\": Show the description of the \"toolname\".\n"
		"-p \"toolname\": Put the file \"toolname\" into the toolbox.\n"
		"-r \"toolname\": Remove the file \"toolname\" from the toolbox.\n"
		"-h: Show this usage help.\n");
}

int load_scripts() {
	DIR *shtb_directory;
	struct dirent *shtb_content;
	int index;
	FILE *test_file;
	char description_string[1045876];
	int file_size;
	char *beginning_pointer, *description_pointer, *second_description_pointer;
	char helper_path[512];
	int read_count;

	home_directory = strdup(getenv("HOME"));
	strcpy(helper_path, home_directory);
	strcat(helper_path, "/.shtb");
	shtb_directory = opendir(helper_path);
	if (shtb_directory == NULL) {
		fprintf(stderr, "%s %s:\n", "Error with ", helper_path);
		perror("opendir");
		return 0;
	}

	while (shtb_content = readdir(shtb_directory)) {
		if (shtb_content->d_type == DT_REG) {
			++file_count;
		}
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
				fprintf(stderr, "%s %s:\n", "Error with ", helper_path);
				perror("fopen");
				return 2;
			}

			fseek(test_file, 0, SEEK_END);
			file_size = ftell(test_file);
			rewind(test_file);
			if ((read_count = fread(description_string, 1, file_size, test_file)) != file_size) {
				fprintf(stderr, "%s %s:\n", "Error with ", helper_path);
				perror("fread");
				return 3;
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