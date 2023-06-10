#ifndef ENTRY_WITH_STAT_H
#define ENTRY_WITH_STAT_H

#include <dirent.h>

#include "types.h"

struct LocalEntry * entry_new(struct dirent *entry);

#endif
