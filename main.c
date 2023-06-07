#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

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
}
