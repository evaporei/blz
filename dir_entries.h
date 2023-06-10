#ifndef DIR_ENTRIES_H
#define DIR_ENTRIES_H

#include "types.h"

#define ENTRIES_INIT_CAPACITY 16

struct DirEntries * dir_entries_new(char *foldername);

void dir_entries_grow(struct DirEntries *dir_entries);

void dir_entries_append(struct DirEntries *dir_entries, struct EntryWithStat entry_with_stat);

void dir_entries_sort(struct DirEntries *dir_entries);

void dir_entries_print(struct DirEntries *dir_entries, struct Args args, int result_list_len);

void dir_entries_free(struct DirEntries *dir_entries);

#endif
