#include <stdio.h>
#include <stdlib.h>

#include "error.h"

struct Error * error_new(char *msg, enum ErrKind kind) {
  struct Error *err = malloc(sizeof(struct Error));

  if (err == NULL) {
    perror("blz: memory allocation error (err)");
    exit(1);
  }

  err->msg = msg;
  err->kind = kind;

  return err;
}
