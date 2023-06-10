#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// concatenate paths
// eg:
// - "/home/rei/" + "ssba" = "/home/rei/ssba"
// - "/home/rei" + ssba" = "/home/rei/ssba" (adds '/')
//
// remember to free this string
char * str_path_cat(char *folder, char *file) {
  char *full_path;

  if (strcmp(folder, ".") != 0) {
    int has_trailing_slash = folder[strlen(folder) - 1] == '/';
    int full_path_len = (strlen(folder) + !has_trailing_slash + strlen(file) + 1);

    full_path = malloc(full_path_len * sizeof(char));

    if (full_path == NULL) {
      perror("blz: memory allocation error 1 (full_path)");
      exit(1);
    }

    strcpy(full_path, folder);
    if (!has_trailing_slash) {
      strcat(full_path, "/");
    }
    strcat(full_path, file);
    full_path[full_path_len - 1] = '\0';
  } else {
    full_path = malloc((strlen(file) + 1) * sizeof(char));

    if (full_path == NULL) {
      perror("blz: memory allocation error 2 (full_path)");
      exit(1);
    }

    strcpy(full_path, file);
    full_path[strlen(file)] = '\0';
  }

  return full_path;
}
