#ifndef RESULT_LIST_H
#define RESULT_LIST_H

#include "types.h"

#define RESULTS_INIT_CAPACITY 2

struct ResultList result_list_new();

void result_list_grow(struct ResultList *results);

void result_list_append_dir_entries(struct ResultList *results, struct DirEntries *dir_entries);

void result_list_append_filename(struct ResultList *results, char *filename);

void result_list_append_err(struct ResultList *results, struct Error *err);

void result_list_print(struct ResultList results, struct Args args);

void result_list_free(struct ResultList results);

#endif
