#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

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
