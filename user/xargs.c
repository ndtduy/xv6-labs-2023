#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void 
xargs_exec(char **xargv) {
	int pid = fork();
	if (pid < 0) {
		fprintf(2, "Error: Fork system call error\n");
		exit(1);
	}

	if (pid == 0) {
		exec(xargv[0], xargv);
		exit(0);
	}
	else wait(0);
}

int
main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(2, "Usage: Run command from standard input\n");
		exit(1);
	}

	char *xargv[MAXARG];
	int xargc = argc - 1;
	memset(xargv, 0, sizeof(xargv));
	int i;
	for (i = 0; i < xargc; i++) {
		xargv[i] = argv[i + 1];
	}

	char buf[512];
	struct stat st;
	if (fstat(0, &st) < 0) { // if cannot stat from standard input
		i = 0;
		memset(buf, 0, sizeof(buf));
		xargv[xargc] = buf;
		while (read(0, &buf[i], 1) > 0) { // read a byte from standard input
			if (buf[i] == '\n') {
				buf[i] = '\0';
				xargs_exec(xargv);
				i = 0;
				memset(buf, 0, sizeof(buf));
			}
			else i++;
		}

		if (buf[i] != 0) xargs_exec(xargv);
	}
	else xargs_exec(xargv);
	exit(0);
}