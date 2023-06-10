#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "types.h"
#include "string.h"

// copy entry data to avoid corruption issues
//
// ```
// The application shall not modify the structure to which the return
// value of readdir() points, nor any storage areas pointed to by
// pointers within the structure. The returned pointer, and pointers
// within the structure, might be invalidated or the structure or the
// storage areas might be overwritten by a subsequent call to readdir()
// on the same directory stream. They shall not be affected by a call
// to readdir() on a different directory stream.
// ```
struct LocalEntry * entry_new(struct dirent *entry) {
  struct LocalEntry *local_entry = malloc(sizeof(struct LocalEntry));

  if (local_entry == NULL) {
    perror("blz: memory allocation error (local_entry)");
    exit(1);
  }

  local_entry->d_type = entry->d_type;
  local_entry->d_name = malloc((strlen(entry->d_name) + 1) * sizeof(char));
  strcpy(local_entry->d_name, entry->d_name);
  local_entry->d_name[strlen(entry->d_name)] = '\0';

  return local_entry;
}

struct stat * stat_new(char *full_path) {
  struct stat *file_stat = malloc(sizeof(struct stat));

  if (stat(full_path, file_stat) != 0) {
    perror("blz: failed to get file status (stat)");
    return NULL;
  }

  return file_stat;
}

struct EntryWithStat entry_with_stat_new(char *arg_foldername, struct dirent *entry) {
  struct EntryWithStat entry_with_stat;

  struct LocalEntry *local_entry = entry_new(entry);

  entry_with_stat.entry = local_entry;

  char *full_path = str_path_cat(arg_foldername, entry->d_name);

  struct stat *file_stat = stat_new(full_path);

  free(full_path);

  entry_with_stat.stat = file_stat;

  return entry_with_stat;
}

// used for qsort in dir_entries
int compare_entries(const void *a, const void *b) {
  struct EntryWithStat *entry_a = (struct EntryWithStat *) a;
  struct EntryWithStat *entry_b = (struct EntryWithStat *) b;
  return strcmp(entry_a->entry->d_name, entry_b->entry->d_name);
}
