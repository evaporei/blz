#ifndef ENTRY_WITH_STAT_H
#define ENTRY_WITH_STAT_H

#include <dirent.h>
#include <sys/stat.h>

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

struct LocalEntry * entry_new(struct dirent *entry);

struct stat * stat_new(char *full_path);

struct EntryWithStat entry_with_stat_new(char *arg_foldername, struct dirent *entry);

int compare_entries(const void *a, const void *b);

void entry_with_stat_print(struct EntryWithStat entry_with_stat, struct Args args);

#endif
