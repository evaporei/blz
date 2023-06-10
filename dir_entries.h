#ifndef DIR_ENTRIES_H
#define DIR_ENTRIES_H

#define ENTRIES_INIT_CAPACITY 16

struct DirEntries * dir_entries_new(char *foldername);

void dir_entries_grow(struct DirEntries *dir_entries);

#endif
