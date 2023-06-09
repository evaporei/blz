#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "types.h"

#define RESULTS_INIT_CAPACITY 2
#define ENTRIES_INIT_CAPACITY 16

int compare_entries(const void *a, const void *b) {
  struct EntryWithStat *entry_a = (struct EntryWithStat *) a;
  struct EntryWithStat *entry_b = (struct EntryWithStat *) b;
  return strcmp(entry_a->entry->d_name, entry_b->entry->d_name);
}

int main(int argc, char* argv[]) {
  char **foldernames;
  struct Flags flags = {
    .all = false,
    .long_list_fmt = false
  };

  int i;
  for (i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (arg[0] == '-') {
      switch (arg[1]) {
        case 'a':
          flags.all = true;
          break;
        case 'l':
          flags.long_list_fmt = true;
          break;
        default:
          printf("blz: (warning) unknown flag '-%c'\n", arg[1]);
          break;
      }
    } else {
      // folder/file names instead of flags
      break;
    }
  }
  foldernames = &argv[i];
  argc -= i;

  if (argc == 0) {
    foldernames[0] = ".";
    argc = 1;
  }

  struct ResultList results;

  results.len = 0;
  results.cap = RESULTS_INIT_CAPACITY;
  results.items = malloc(results.cap * sizeof(struct EntryResult));

  if (results.items == NULL) {
    perror("blz: memory allocation error (list.items)");
    exit(1);
  }

  // get the data using sys/lib calls
  for (int i = 0; i < argc; i++) {
    if (results.len >= results.cap) {
      results.cap *= 2;
      results.items = realloc(results.items, results.cap * sizeof(struct EntryResult));

      if (results.items == NULL) {
        perror("blz: memory reallocation error (results.items)");
        exit(1);
      }
    }

    if (access(foldernames[i], F_OK) != 0) {
      char *msg_template = "blz: cannot access '%s': No such file or directory";
      char *msg = malloc((strlen(msg_template) + strlen(foldernames[i]) - 1) * sizeof(char));
      sprintf(msg, msg_template, foldernames[i]);

      struct Error *err = malloc(sizeof(struct Error));
      err->msg = msg;
      err->kind = NoEntity;

      results.items[results.len].dir_entries = NULL;
      results.items[results.len].filename = NULL;
      results.items[results.len].err = err;
      results.len++;
      continue;
    }

    DIR *dir = opendir(foldernames[i]);

    // it is actually a file
    if (dir == NULL) {
      results.items[results.len].dir_entries = NULL;
      // though this allocation/copy is not necessary
      // I left it so that all fields from the big struct
      // have to be freed.
      results.items[results.len].filename = malloc(strlen(foldernames[i]) * sizeof(char));
      strcpy(results.items[results.len].filename, foldernames[i]);
      results.items[results.len].err = NULL;
      results.len++;
      continue;
    }

    struct DirEntries *dir_entries = malloc(sizeof(struct DirEntries));

    dir_entries->foldername = foldernames[i];
    dir_entries->total_blocks = 0;
    dir_entries->ent_len = 0;
    dir_entries->ent_cap = ENTRIES_INIT_CAPACITY;
    dir_entries->entries = malloc(dir_entries->ent_cap * sizeof(struct EntryWithStat));

    if (dir_entries->entries == NULL) {
      perror("blz: memory allocation error (dir_entries->entries)");
      exit(1);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
      if (dir_entries->ent_len >= dir_entries->ent_cap) {
        dir_entries->ent_cap *= 2;
        dir_entries->entries = realloc(dir_entries->entries, dir_entries->ent_cap * sizeof(struct EntryWithStat));

        if (dir_entries->entries == NULL) {
          perror("blz: memory reallocation error (dir_entries->entries)");
          exit(1);
        }
      }

      if (entry->d_name[0] == '.' && !flags.all) {
        continue;
      }

      // copy entry data to avoid corruption issues
      struct LocalEntry *local_entry = malloc(sizeof(struct LocalEntry));

      local_entry->d_type = entry->d_type;
      local_entry->d_name = malloc((strlen(entry->d_name) + 1) * sizeof(char));
      strcpy(local_entry->d_name, entry->d_name);
      local_entry->d_name[strlen(entry->d_name)] = '\0';

      dir_entries->entries[dir_entries->ent_len].entry = local_entry;

      char *full_path;

      if (strcmp(foldernames[i], ".") != 0) {
        int has_trailing_slash = foldernames[i][strlen(foldernames[i]) - 1] == '/';
        int full_path_len = (strlen(foldernames[i]) + !has_trailing_slash + strlen(entry->d_name) + 1);

        full_path = malloc(full_path_len * sizeof(char));

        if (full_path == NULL) {
          perror("blz: memory allocation error 1 (full_path)");
          exit(1);
        }

        strcpy(full_path, foldernames[i]);
        if (!has_trailing_slash) {
          strcat(full_path, "/");
        }
        strcat(full_path, entry->d_name);
        full_path[full_path_len - 1] = '\0';
      } else {
        full_path = malloc((strlen(entry->d_name) + 1) * sizeof(char));

        if (full_path == NULL) {
          perror("blz: memory allocation error 2 (full_path)");
          exit(1);
        }

        strcpy(full_path, entry->d_name);
        full_path[strlen(entry->d_name)] = '\0';
      }

      struct stat *file_stat = malloc(sizeof(struct stat));

      if (stat(full_path, file_stat) != 0) {
        perror("blz: failed to get file status (stat)");
        dir_entries->entries[dir_entries->ent_len].stat = NULL;
      } else {
        dir_entries->total_blocks += (file_stat->st_blocks * 512) / 1024;
        dir_entries->entries[dir_entries->ent_len].stat = file_stat;
      }

      free(full_path);

      dir_entries->ent_len++;
    }

    // order/sort entries
    qsort(dir_entries->entries, dir_entries->ent_len, sizeof(struct EntryWithStat), compare_entries);

    results.items[results.len].dir_entries = dir_entries;
    results.items[results.len].filename = NULL;
    results.items[results.len].err = NULL;
    results.len++;

    int close_res = closedir(dir);

    if (close_res < 0) {
      perror("blz: failed to close directory (closedir)");
    }
  }

  // print gathered data
  for (int i = 0; i < results.len; i++) {
    if (results.items[i].dir_entries != NULL) {
      struct DirEntries *dir_entries = results.items[i].dir_entries;

      if (flags.long_list_fmt) {
        if (results.len > 1) {
          printf("%s:\n", dir_entries->foldername);
        }
        printf("total: %llu\n", dir_entries->total_blocks);
      }

      for (int j = 0; j < dir_entries->ent_len; j++) {
        struct EntryWithStat *entry_with_stat = &(dir_entries->entries[j]);
        struct LocalEntry *entry = entry_with_stat->entry;
        struct stat *stat = entry_with_stat->stat;

        if (flags.long_list_fmt) {
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

        if (flags.long_list_fmt) {
          printf("\n");
        } else {
          printf(" ");
        }
      }

      if (!flags.long_list_fmt) {
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

  // free data
  for (int i = 0; i < results.len; i++) {
    if (results.items[i].dir_entries != NULL) {
      struct DirEntries *dir_entries = results.items[i].dir_entries;

      for (int j = 0; j < dir_entries->ent_len; j++) {
        struct EntryWithStat *entry_with_stat = &(dir_entries->entries[j]);
        struct LocalEntry *entry = entry_with_stat->entry;
        struct stat *stat = entry_with_stat->stat;

        free(stat);
        free(entry);
      }

      free(dir_entries);
    }

    if (results.items[i].filename != NULL) {
      free(results.items[i].filename);
    }

    if (results.items[i].err != NULL) {
      free(results.items[i].err->msg);
      free(results.items[i].err);
    }
  }

  free(results.items);
}
