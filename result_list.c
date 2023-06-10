#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "result_list.h"
#include "types.h"

struct ResultList result_list_new() {
  struct ResultList results;

  results.len = 0;
  results.cap = RESULTS_INIT_CAPACITY;
  results.items = malloc(results.cap * sizeof(struct EntryResult));

  if (results.items == NULL) {
    perror("blz: memory allocation error (list.items)");
    exit(1);
  }
  
  return results;
}

void result_list_grow(struct ResultList *results) {
  if (results->len >= results->cap) {
    results->cap *= 2;
    results->items = realloc(results->items, results->cap * sizeof(struct EntryResult));

    if (results->items == NULL) {
      perror("blz: memory reallocation error (results.items)");
      exit(1);
    }
  }
}

void result_list_append_dir_entries(struct ResultList *results, struct DirEntries *dir_entries) {
  results->items[results->len].dir_entries = dir_entries;
  results->items[results->len].filename = NULL;
  results->items[results->len].err = NULL;
  results->len++;
}

void result_list_append_filename(struct ResultList *results, char *filename) {
  results->items[results->len].dir_entries = NULL;
  results->items[results->len].filename = malloc(strlen(filename) * sizeof(char));
  strcpy(results->items[results->len].filename, filename);
  results->items[results->len].err = NULL;
  results->len++;
}

void result_list_append_err(struct ResultList *results, struct Error *err) {
  results->items[results->len].dir_entries = NULL;
  results->items[results->len].filename = NULL;
  results->items[results->len].err = err;
  results->len++;
}
