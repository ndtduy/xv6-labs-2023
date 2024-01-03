#include "kernel/types.h"
#include "user/user.h"

void
filter(int *oldp)
{
  int n, buf;
  int newp[2];
  pipe(newp);
  close(oldp[1]);

  // read will false when reach the last descriptor.
  if (read(oldp[0], &n, sizeof(int))) {
    printf("prime %d\n", n);
    int pId = fork() ;
    if (pId == 0) {
      filter(newp);
      exit(0);
    } 

    while (read(oldp[0], &buf, sizeof(int))) {
      if (buf % n != 0) {
        write(newp[1], &buf, sizeof(int));
      }
    }
    // decrement count of descriptor reference
    close(newp[0]);
    close(newp[1]); 

    // wait for OS release "zombie" process
    wait(0);
  }
  close(oldp[0]);

  exit(0);
}

int
main(int argc, char *argv[])
{
  if (argc > 1) {
    fprintf(2, "Usage: Prints primes less than 35 \n");
    exit(1);
  }

  int p[2];
  pipe(p);

  if (fork() == 0) {
    filter(p);
    exit(0);
  } 

  for (int i = 2; i <= 35; ++i) {
    write(p[1], &i, sizeof(int));
  }
  close(p[1]);
  close(p[0]);
  wait(0);
  exit(0);
}