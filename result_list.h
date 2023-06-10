#ifndef RESULT_LIST_H
#define RESULT_LIST_H

#include "cli.h"
#include "dir_entries.h"
#include "error.h"

#define RESULTS_INIT_CAPACITY 2

/* either a dir_entries list, a file name or an error */
struct EntryResult {
  struct DirEntries *dir_entries;
  char *filename;
  struct Error *err;
};

struct ResultList {
  struct EntryResult *items;
  int len;
  int cap;
};

struct ResultList result_list_new();

void result_list_grow(struct ResultList *results);

void result_list_append_dir_entries(struct ResultList *results, struct DirEntries *dir_entries);

void result_list_append_filename(struct ResultList *results, char *filename);

void result_list_append_err(struct ResultList *results, struct Error *err);

void result_list_print(struct ResultList results, struct Args args);

void result_list_free(struct ResultList results);

#endif
