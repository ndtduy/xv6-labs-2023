#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(2, "Usage: Sleep for a while...");
		exit(1);
	}

	if (sleep(atoi(argv[1])) == -1) {
		fprintf(2, "Error: Sleep system call error...");
		exit(1);
	}
	exit(0);
}