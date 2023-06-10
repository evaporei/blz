#ifndef ENTRY_WITH_STAT_H
#define ENTRY_WITH_STAT_H

#include <dirent.h>

#include "types.h"

struct LocalEntry * entry_new(struct dirent *entry);

struct stat * stat_new(char *full_path);

struct EntryWithStat entry_with_stat_new(char *arg_foldername, struct dirent *entry);

int compare_entries(const void *a, const void *b);

void entry_with_stat_print(struct EntryWithStat entry_with_stat, struct Args args);

#endif
