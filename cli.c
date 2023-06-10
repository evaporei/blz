#include <stdbool.h>
#include <stdio.h>

#include "cli.h"

struct Args parse_args(int argc, char* argv[]) {
  struct Flags flags = {
    .all = false,
    .long_list_fmt = false
  };

  int i;
  for (i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (arg[0] == '-') {
      switch (arg[1]) {
        case 'a':
          flags.all = true;
          break;
        case 'l':
          flags.long_list_fmt = true;
          break;
        default:
          printf("blz: (warning) unknown flag '-%c'\n", arg[1]);
          break;
      }
    } else {
      // folder/file names instead of flags
      break;
    }
  }
  char **foldernames = &argv[i];
  int folders_len = argc - i;

  if (folders_len == 0) {
    foldernames[0] = ".";
    folders_len = 1;
  }

  struct Args args = {
    .foldernames = foldernames,
    .folders_len = folders_len,
    .flags = flags
  };

  return args;
}
