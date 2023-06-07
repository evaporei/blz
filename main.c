#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
  char* foldername;

  if (argc == 1) {
    foldername = ".";
  } else if (argc == 2) {
    foldername = argv[1];
  } else {
    fprintf(stderr, "blz doesn't support multiple folders yet\n");
    exit(1);
  }

  DIR *dir = opendir(foldername);

  // it is actually a file
  if (dir == NULL) {
    printf("%s\n", foldername);
    exit(0);
  }

  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] != '.') {
      // print file name itself
      printf("%s", entry->d_name);

      // print file type suffix
      switch (entry->d_type) {
        case DT_DIR:
          printf("/");
          break;
        case DT_FIFO:
          printf("|");
          break;
        case DT_LNK:
          printf("@");
          break;
        case DT_REG:
          struct stat file_stat;

          if (stat(entry->d_name, &file_stat) != 0) {
            perror("Failed to get file status");
            exit(errno);
          }

          // file is executable
          if (file_stat.st_mode & S_IXUSR) {
            printf("*");
          }

          break;
        case DT_SOCK:
          printf("=");
          break;
      }

      // space in between files
      printf(" ");
    }
  }

  printf("\n");

  int close_res = closedir(dir);

  if (close_res < 0) {
    exit(errno);
  }
}
