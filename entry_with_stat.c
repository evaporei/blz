#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "args.h"
#include "entry_with_stat.h"
#include "string.h"

// copy entry data to avoid corruption issues
//
// ```
// The application shall not modify the structure to which the return
// value of readdir() points, nor any storage areas pointed to by
// pointers within the structure. The returned pointer, and pointers
// within the structure, might be invalidated or the structure or the
// storage areas might be overwritten by a subsequent call to readdir()
// on the same directory stream. They shall not be affected by a call
// to readdir() on a different directory stream.
// ```
struct LocalEntry * entry_new(struct dirent *entry) {
  struct LocalEntry *local_entry = malloc(sizeof(struct LocalEntry));

  if (local_entry == NULL) {
    perror("blz: memory allocation error (local_entry)");
    exit(1);
  }

  local_entry->d_type = entry->d_type;
  local_entry->d_name = malloc((strlen(entry->d_name) + 1) * sizeof(char));
  strcpy(local_entry->d_name, entry->d_name);
  local_entry->d_name[strlen(entry->d_name)] = '\0';

  return local_entry;
}

struct stat * stat_new(char *full_path) {
  struct stat *file_stat = malloc(sizeof(struct stat));

  if (stat(full_path, file_stat) != 0) {
    perror("blz: failed to get file status (stat)");
    return NULL;
  }

  return file_stat;
}

struct EntryWithStat entry_with_stat_new(char *arg_foldername, struct dirent *entry) {
  struct EntryWithStat entry_with_stat;

  struct LocalEntry *local_entry = entry_new(entry);

  entry_with_stat.entry = local_entry;

  char *full_path = str_path_cat(arg_foldername, entry->d_name);

  struct stat *file_stat = stat_new(full_path);

  free(full_path);

  entry_with_stat.stat = file_stat;

  return entry_with_stat;
}

// used for qsort in dir_entries
int compare_entries(const void *a, const void *b) {
  struct EntryWithStat *entry_a = (struct EntryWithStat *) a;
  struct EntryWithStat *entry_b = (struct EntryWithStat *) b;
  return strcmp(entry_a->entry->d_name, entry_b->entry->d_name);
}

void entry_with_stat_print(struct EntryWithStat entry_with_stat, struct Args args) {
  struct LocalEntry *entry = entry_with_stat.entry;
  struct stat *stat = entry_with_stat.stat;

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
