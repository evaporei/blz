#include <stdio.h>
#include <stdlib.h>

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
