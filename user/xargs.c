#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int pid;
  int i;

  const char *cmd = argv[1];

  char str[100];
  while (1) {
    gets(str, 100);
    // printf("input:%s", str);
    if (str[0] == '\0')
      break;
    char *c = &str[strlen(str) - 1];
    if (*c == '\n' || *c == '\r')
      *c = '\0';

    pid = fork();
    if (pid == 0) {
      /* 子进程 */
      char *args[MAXARG];
      for (i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
      }
      args[argc - 1] = str /* input */;
      args[argc] = (char *)0;
      exec((char *)cmd, args);
    } else {
      wait((int *)0);
    }
  }

  exit(0);
}
