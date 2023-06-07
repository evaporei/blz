#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
  char **foldernames = argv;

  if (argc == 1) {
    foldernames[0] = ".";
  } else {
    foldernames = &argv[1];
    argc -= 1;
  }

  for (int i = 0; i < argc; i++) {
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
              perror("Failed to get file status");
              exit(errno);
            }

            free(full_path);

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
      perror("Failed to close directory");
      exit(errno);
    }
  }
}
