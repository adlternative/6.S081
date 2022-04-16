#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *concat(const char *dir, const char *file) {
  char *path = malloc(strlen(dir) + strlen(file) + 2);
  char *p = path;
  memcpy(p, dir, strlen(dir));
  p += strlen(dir);
  *p++ = '/';
  memcpy(p, file, strlen(file));
  p += strlen(file);
  *p++ = '\0';
  return path;
}

void find(const char *dir, const char *file) {
  int fd;
  struct stat st;
  struct dirent de;

  char *sub_dir[10];
  int sub_dir_count = 0;

  int i;

  if ((fd = open(dir, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", dir);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", dir);
    close(fd);
    return;
  }

  if (st.type != T_DIR) {
    fprintf(2, "find: %s is not a dir\n", dir);
    close(fd);
    return;
  }

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    char *sub_path;
    if (de.inum == 0 || !strcmp( de.name, ".") || !strcmp( de.name, ".."))
      continue;

    if (strcmp(de.name, file) == 0)
      printf("%s/%s\n", dir, file);

    sub_path = concat(dir, de.name);
    if (stat(sub_path, &st) < 0) {
      printf("ls: cannot stat %s\n", sub_path);
      free(sub_path);
      continue;
    }

    if (st.type == T_DIR) {
      sub_dir[sub_dir_count++] = sub_path;
    }
  }
  for (i = 0; i < sub_dir_count; i++) {
    // printf("search:%s\n", sub_dir[i]);
    find(sub_dir[i], file);
    free(sub_dir[i]);
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(2, "Usage: %s <dir> <file>\n", argv[0]);
    exit(0);
  }

  find(argv[1], argv[2]);

  exit(0);
}
