#include "kernel/types.h"
#include "user/user.h"

void son_handle(int *fds /* 和父进程通信 */) {
  int fds2[2] = {-1, -1};
  int value;
  int prime;
  int pid2 = 0;

  /* 子进程 初始化 */
  /* 子进程 关写 */
  close(fds[1]);

  int ret = read(fds[0], &prime, sizeof(int));
  if (ret < 0) {
    fprintf(2, "prime: read failed\n line: %d\n", __LINE__);
    exit(1);
  } else if (!ret) {
    close(fds[0]);
    return;
  }
  // sleep(1);
  // printf("pid[%d] read %d\n", getpid(), prime);
  printf("prime %d\n", prime);

  while (1) {
    /* 读后面的数字 */
    ret = read(fds[0], &value, sizeof(int));
    if (ret < 0) {
      fprintf(2, "prime: read failed\n line: %d\n", __LINE__);
      exit(1);
    } else if (!ret) {
      /* 读到 0 关闭读*/
      close(fds[0]);
      /* 读到 0 关闭对子进程写 */
      if (fds2[1] != -1) {
        close(fds2[1]);
        wait((int *)0);
      }
      return;
    }
    // printf("pid[%d] read %d\n", getpid(), value);

    /* 非整除则传递给子进程 */
    if (value % prime) {

      if (fds2[0] == -1) {
        // pipe + fork
        if (pipe(fds2) < 0) {
          fprintf(2, "prime: pipe failed\n line: %d\n", __LINE__);
          exit(1);
        }
        pid2 = fork();
      }
      if (pid2 == 0) {
        /* 子进程 */
        son_handle(fds2);
      } else {
        /* 父进程 */
        close(fds2[0]);
        // printf("!!!fds2[1]=%d\n", fds2[1]);
        ret = write(fds2[1], &value, sizeof(int));
        if (ret <= 0) {
          fprintf(2, "prime: write failed\n line: %d\n fds2[1]:%d\n", __LINE__,
                  fds2[1]);
          exit(1);
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int pid;
  int i;
  int fds[2];

  if (pipe(fds) < 0) {
    fprintf(2, "prime: pipe failed\n line: %d\n", __LINE__);
    exit(1);
  }
  pid = fork();
  if (pid == 0) {
    /* 子进程 */
    son_handle(fds);
  } else {
    /* 父进程 关读 */
    close(fds[0]);
    for (i = 2; i < 35; i++) {
      // printf("write %d\n", i);
      int ret = write(fds[1], &i, sizeof(int));
      if (ret != sizeof(int)) {
        fprintf(2, "prime: write failed\n line: %d\n", __LINE__);
        exit(1);
      }
    }
    // printf("close!");
    /* 关写 */
    close(fds[1]);
    wait((int *)0);
  }

  exit(0);
}
