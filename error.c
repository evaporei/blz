#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

struct Error * error_new(char *file_or_dir) {
  char *msg_template = "blz: cannot access '%s': No such file or directory";
  char *msg = malloc((strlen(msg_template) + strlen(file_or_dir) - 1) * sizeof(char));

  if (msg == NULL) {
    perror("blz: memory allocation error (msg)");
    exit(1);
  }

  sprintf(msg, msg_template, file_or_dir);

  struct Error *err = malloc(sizeof(struct Error));

  if (err == NULL) {
    perror("blz: memory allocation error (err)");
    exit(1);
  }

  err->msg = msg;
  err->kind = NoEntity;

  return err;
}
