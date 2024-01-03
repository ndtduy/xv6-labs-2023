#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path) {
	static char buf[DIRSIZ + 1];
	char *p;

	for (p = path + strlen(path); p >= path && *p != '/'; p--);
	p++;
	
	if (strlen(p) >= DIRSIZ) {
		return p;
	}

	memset(buf, 0, sizeof(buf));
	memmove(buf, p, strlen(p));

	return buf;
}

void 
find(char *path, char *filename) {
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if ((fd = open(path, O_RDONLY)) < 0) {
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if (fstat(fd, &st) < 0) {
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch (st.type) {
	case T_DEVICE:
	case T_FILE:
		fprintf(2, "find: %s is not a directory\n", path);
		break;
	case T_DIR:
		if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
			printf("find: path too long\n");
			break;
		}

		strcpy(buf, path);
		p = buf + strlen(buf);
		*p++ = '/';

		while (read(fd, &de, sizeof(de)) == sizeof(de)) {
			if (de.inum == 0) continue;
			if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
			
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = '\0';
			if (stat(buf, &st) < 0) {
				fprintf(2, "find: cannot stat %s\n", buf);
				continue;
			}

			if (st.type == T_DIR) {
				find(buf, filename);
			}
			else if (st.type == T_DEVICE || st.type == T_FILE) {
				if (strcmp(fmtname(buf), filename) == 0) {
					printf("%s\n", buf);
				}
			}
		}
		break;
	}
	close(fd);
}

int
main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(2, "Usage: Find all files with specific name...\n");
		exit(1);
	}

	find(argv[1], argv[2]);
	exit(0);
}