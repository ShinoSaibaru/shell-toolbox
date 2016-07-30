#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>

#include "declarations.h"

int execute(char *arguments[]) {
	pid_t parent_id;
	int status;

	parent_id = fork();
	if (parent_id < 0) {
		fprintf(stderr, "%s\n", "Couldn't create a child process for tool execution.");
		return -1;
	} else if (parent_id > 0) {
		waitpid(parent_id, &status, 0);
		return status;
	}

	execv(arguments[0], arguments + 1);
}