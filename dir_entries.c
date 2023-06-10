#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "dir_entries.h"
#include "entry_with_stat.h"
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
  if (entry_with_stat.stat != NULL) {
    dir_entries->total_blocks += (entry_with_stat.stat->st_blocks * 512) / 1024;
  }
  dir_entries->ent_len++;
}

void dir_entries_sort(struct DirEntries *dir_entries) {
  qsort(dir_entries->entries, dir_entries->ent_len, sizeof(struct EntryWithStat), compare_entries);
}

void dir_entries_print(struct DirEntries *dir_entries, struct Args args, int result_list_len) {
  if (args.flags.long_list_fmt) {
    if (result_list_len > 1) {
      printf("%s:\n", dir_entries->foldername);
    }
    printf("total: %llu\n", dir_entries->total_blocks);
  }

  for (int j = 0; j < dir_entries->ent_len; j++) {
    struct EntryWithStat entry_with_stat = dir_entries->entries[j];

    entry_with_stat_print(entry_with_stat, args);
  }

  if (!args.flags.long_list_fmt) {
    printf("\n");
  }
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
