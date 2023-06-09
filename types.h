#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

enum ErrKind {
  NoEntity = 1 // ENOENT
};

struct Error {
  char *msg;
  enum ErrKind kind;
};

// this struct only exists because stupid `readdir`
// doesn't give owned data to us, only references to
// static data
//
// and since it can do anything with its static data,
// sometimes this `ls` was printing corrupted file names
// etc
struct LocalEntry {
  unsigned char d_type;
  char *d_name;
};

struct EntryWithStat {
  struct LocalEntry *entry;
  struct stat *stat; // can be None
};

struct DirEntries {
  char *foldername;
  unsigned long long total_blocks;
  struct EntryWithStat *entries;
  int ent_len;
  int ent_cap;
};

/* either a dir_entries list, a file name or an error */
struct EntryResult {
  struct DirEntries *dir_entries;
  char *filename;
  struct Error *err;
};

struct ResultList {
  struct EntryResult *items;
  int len;
  int cap;
};

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

#endif
