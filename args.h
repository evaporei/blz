#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

struct Flags {
  // to show dot files/folders
  bool all; // -a
  // to show detailed data from each file and folder (permissions, size, etc)
  bool long_list_fmt; // -l
};

struct Args {
  char **foldernames;
  int folders_len; // number of folders
  struct Flags flags;
};

struct Args parse_args(int argc, char* argv[]);

#endif
