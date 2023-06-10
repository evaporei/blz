#ifndef RESULT_LIST_H
#define RESULT_LIST_H

#define RESULTS_INIT_CAPACITY 2

struct ResultList result_list_new();

void result_list_grow(struct ResultList *results);

#endif
