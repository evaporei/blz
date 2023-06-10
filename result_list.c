#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

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

void result_list_print(struct ResultList results, struct Args args) {
  for (int i = 0; i < results.len; i++) {
    if (results.items[i].dir_entries != NULL) {
      struct DirEntries *dir_entries = results.items[i].dir_entries;

      if (args.flags.long_list_fmt) {
        if (results.len > 1) {
          printf("%s:\n", dir_entries->foldername);
        }
        printf("total: %llu\n", dir_entries->total_blocks);
      }

      for (int j = 0; j < dir_entries->ent_len; j++) {
        struct EntryWithStat *entry_with_stat = &(dir_entries->entries[j]);
        struct LocalEntry *entry = entry_with_stat->entry;
        struct stat *stat = entry_with_stat->stat;

        if (args.flags.long_list_fmt) {
          // print file permissions
          printf(S_ISDIR(stat->st_mode) ? "d" : "-");
          printf(stat->st_mode & S_IRUSR ? "r" : "-");
          printf(stat->st_mode & S_IWUSR ? "w" : "-");
          printf(stat->st_mode & S_IXUSR ? "x" : "-");
          printf(stat->st_mode & S_IRGRP ? "r" : "-");
          printf(stat->st_mode & S_IWGRP ? "w" : "-");
          printf(stat->st_mode & S_IXGRP ? "x" : "-");
          printf(stat->st_mode & S_IROTH ? "r" : "-");
          printf(stat->st_mode & S_IWOTH ? "w" : "-");
          printf(stat->st_mode & S_IXOTH ? "x" : "-");
          printf(" ");

          // print number of hard links
          printf("%*lld ", sizeof(stat->st_nlink), stat->st_nlink);

          struct passwd *pwd = getpwuid(stat->st_uid);

          // print user name (owner)
          if (pwd != NULL) {
            printf("%s ", pwd->pw_name);
          } else {
            printf("(unknown) ");
          }

          struct group *grp = getgrgid(stat->st_gid);

          // print group name
          if (grp != NULL) {
            printf("%s ", grp->gr_name);
          } else {
            printf("(unknown) ");
          }

          // print file size
          printf("%*lld ", sizeof(stat->st_size), stat->st_size);

          // get and format date of the last modification
          time_t mod_time = stat->st_mtime;
          struct tm *time_info = localtime(&mod_time);

          char time_str[20];
          strftime(time_str, sizeof(time_str), "%b %e %H:%M", time_info);

          // print date of the last modification
          printf("%s ", time_str);
        }

        printf("%s", entry->d_name);

        // print file type suffix
        switch (entry->d_type) {
          case DT_DIR:
            printf("/");
            break;
          case DT_FIFO:
            printf("|");
            break;
          case DT_LNK:
            printf("@");
            break;
          case DT_REG:
            // file is executable
            if (stat->st_mode & S_IXUSR) {
              printf("*");
            }
            break;
          case DT_SOCK:
            printf("=");
            break;
        }

        if (args.flags.long_list_fmt) {
          printf("\n");
        } else {
          printf(" ");
        }
      }

      if (!args.flags.long_list_fmt) {
        printf("\n");
      }
    }

    if (results.items[i].filename != NULL) {
      printf("%s\n", results.items[i].filename);
    }

    if (results.items[i].err != NULL) {
      printf("%s\n", results.items[i].err->msg);
    }
  }
}
