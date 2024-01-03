#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv) {
	int p[2];
	pipe(p);

	char buf[1];
	int pid = fork();
	if (pid == 0) {
		read(p[0], buf, 1);
		if (buf[0] == 1) {
			printf("%d: received ping\n", pid);
			buf[0] = 2;
		}
		write(p[1], buf, 1);

		close(p[0]);
		close(p[1]);
		exit(0);
	}
	else {
		buf[0] = 1;
		write(p[1], buf, 1);
		wait(0);
		read(p[0], buf, 1);
		if (buf[0] == 2) {
			printf("%d: received pong\n", pid);
		}

		close(p[0]);
		close(p[1]);
		exit(0);
	}
	exit(0);
}