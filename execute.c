#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>

#include "declarations.h"

int execute(int argc, char *argv[]) {
	pid_t parent_id, session_id;

	parent_id = fork();
	if (parent_id < 0) {
		fprintf(stderr, "%s\n", "Couldn't create a child process for tool execution.");
		return;
	} else if (parent_id > 0) {
		waitpid(parent_id, NULL, 0);
		return 0;
	}

	execv(argv[0], argv + 1);
}