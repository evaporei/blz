#ifndef ENTRY_WITH_STAT_H
#define ENTRY_WITH_STAT_H

#include <dirent.h>

#include "types.h"

struct LocalEntry * entry_new(struct dirent *entry);

struct stat * stat_new(char *full_path);

int compare_entries(const void *a, const void *b);

#endif
