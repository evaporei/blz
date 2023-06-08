#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  char **foldernames;
  // to show dot files/folders
  bool flag_all = false;
  // to show detailed data from each file and folder (permissions, size, etc)
  bool flag_long_list_fmt = false;

  int i;
  for (i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (arg[0] == '-') {
      switch (arg[1]) {
        case 'a':
          flag_all = true;
          break;
        case 'l':
          flag_long_list_fmt = true;
          break;
        default:
          printf("warning: unknown flag '-%c'\n", arg[1]);
          break;
      }
    } else {
      // folder/file names instead of flags
      break;
    }
  }
  foldernames = &argv[i];
  argc -= i;

  if (argc == 0) {
    foldernames[0] = ".";
    argc = 1;
  }

  for (int i = 0; i < argc; i++) {
    unsigned long long total_blocks = 0;

    if (access(foldernames[i], F_OK) != 0) {
      fprintf(stderr, "blz: cannot access '%s': No such file or directory\n", foldernames[i]);
      continue;
    }

    DIR *dir = opendir(foldernames[i]);

    // it is actually a file
    if (dir == NULL) {
      printf("%s\n", foldernames[i]);
      continue;
    }

    if (argc > 1) {
      printf("%s:\n", foldernames[i]);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_name[0] == '.' && !flag_all) {
        continue;
      }

      // print file name itself
      printf("%s", entry->d_name);

      char *full_path;

      if (strcmp(foldernames[i], ".") != 0) {
        int has_trailing_slash = foldernames[i][strlen(foldernames[i]) - 1] == '/';

        full_path = malloc(strlen(foldernames[i]) + !has_trailing_slash + strlen(entry->d_name) + 1);

        /* printf("foldernames[i]: %s\n", foldernames[i]); */
        /* printf("entry->d_name: %s\n", entry->d_name); */

        strcpy(full_path, foldernames[i]);
        if (!has_trailing_slash) {
          strcat(full_path, "/");
        }
        strcat(full_path, entry->d_name);
      } else {
        full_path = malloc(strlen(entry->d_name) + 1);

        strcpy(full_path, entry->d_name);
      }

      struct stat file_stat;

      if (stat(full_path, &file_stat) != 0) {
        /* printf("\nfull_path: %s\n", full_path); */
        free(full_path);
        perror("Failed to get file status (stat)");
      }

      free(full_path);

      total_blocks += (file_stat.st_blocks * 512) / 1024;

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
          // file is executable
          if (file_stat.st_mode & S_IXUSR) {
            printf("*");
          }

          break;
        case DT_SOCK:
          printf("=");
          break;
      }

      // separator for files
      if (flag_long_list_fmt) {
        printf("\n");
      } else {
        printf(" ");
      }
    }

    printf("\n");

    if (flag_long_list_fmt) {
      printf("total: %llu\n", total_blocks);
    }

    int close_res = closedir(dir);

    if (close_res < 0) {
      perror("Failed to close directory (closedir)");
    }
  }
}
