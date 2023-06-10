#ifndef ERROR_H
#define ERROR_H

enum ErrKind {
  NoEntity = 1 // ENOENT
};

struct Error {
  char *msg;
  enum ErrKind kind;
};

struct Error * error_new(char *file_or_dir);

#endif
