#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int pid;
  int fds[2][2];
  char buf[1];

  if (pipe(fds[0]) < 0) {
    printf("pingpong: pipe failed\n");
    exit(1);
  }
  if (pipe(fds[1]) < 0) {
    printf("pingpong: pipe failed\n");
    exit(1);
  }

  pid = fork();

  // 子
  if (pid == 0) {
    read(fds[0][0], buf, sizeof(buf));
    printf("%d: received ping\n", getpid());
    write(fds[1][1], buf, 1);
  } else {
    write(fds[0][1], "t", 1);
    read(fds[1][0], buf, sizeof(buf));
    printf("%d: received pong\n", getpid());
  }

  exit(0);
}
