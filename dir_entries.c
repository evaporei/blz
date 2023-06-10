#include <stdio.h>
#include <stdlib.h>

#include "dir_entries.h"
#include "types.h"

struct DirEntries * dir_entries_new(char *foldername) {
  struct DirEntries *dir_entries = malloc(sizeof(struct DirEntries));

  dir_entries->foldername = foldername;
  dir_entries->total_blocks = 0;
  dir_entries->ent_len = 0;
  dir_entries->ent_cap = ENTRIES_INIT_CAPACITY;
  dir_entries->entries = malloc(dir_entries->ent_cap * sizeof(struct EntryWithStat));

  if (dir_entries->entries == NULL) {
    perror("blz: memory allocation error (dir_entries->entries)");
    exit(1);
  }

  return dir_entries;
}

void dir_entries_grow(struct DirEntries *dir_entries) {
  if (dir_entries->ent_len >= dir_entries->ent_cap) {
    dir_entries->ent_cap *= 2;
    dir_entries->entries = realloc(dir_entries->entries, dir_entries->ent_cap * sizeof(struct EntryWithStat));

    if (dir_entries->entries == NULL) {
      perror("blz: memory reallocation error (dir_entries->entries)");
      exit(1);
    }
  }
}

void dir_entries_append(struct DirEntries *dir_entries, struct EntryWithStat entry_with_stat) {
  dir_entries->entries[dir_entries->ent_len] = entry_with_stat;
  dir_entries->ent_len++;
}

void dir_entries_free(struct DirEntries *dir_entries) {
  for (int j = 0; j < dir_entries->ent_len; j++) {
    struct EntryWithStat entry_with_stat = dir_entries->entries[j];
    struct LocalEntry *entry = entry_with_stat.entry;
    struct stat *stat = entry_with_stat.stat;

    free(entry->d_name);
    free(entry);
    free(stat);
  }

  // // no need to free because this is coming from argv
  // // free(): invalid pointer
  // free(dir_entries->foldername);
  free(dir_entries->entries);
  free(dir_entries);
}
