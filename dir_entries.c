#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "dir_entries.h"
#include "entry_with_stat.h"
#include "types.h"

struct DirEntries * dir_entries_new(char *foldername) {
  struct DirEntries *dir_entries = malloc(sizeof(struct DirEntries));

  dir_entries->foldername = foldername;
  dir_entries->total_blocks = 0;
  dir_entries->ent_len = 0;
  dir_entries->ent_cap = ENTRIES_INIT_CAPACITY;
  dir_entries->entries = malloc(dir_entries->ent_cap * sizeof(struct EntryWithStat));

  if (dir_entries->entries == NULL) {
    perror("blz: memory allocation error (dir_entries->entries)");
    exit(1);
  }

  return dir_entries;
}

void dir_entries_grow(struct DirEntries *dir_entries) {
  if (dir_entries->ent_len >= dir_entries->ent_cap) {
    dir_entries->ent_cap *= 2;
    dir_entries->entries = realloc(dir_entries->entries, dir_entries->ent_cap * sizeof(struct EntryWithStat));

    if (dir_entries->entries == NULL) {
      perror("blz: memory reallocation error (dir_entries->entries)");
      exit(1);
    }
  }
}

void dir_entries_append(struct DirEntries *dir_entries, struct EntryWithStat entry_with_stat) {
  dir_entries->entries[dir_entries->ent_len] = entry_with_stat;
  if (entry_with_stat.stat != NULL) {
    dir_entries->total_blocks += (entry_with_stat.stat->st_blocks * 512) / 1024;
  }
  dir_entries->ent_len++;
}

void dir_entries_sort(struct DirEntries *dir_entries) {
  qsort(dir_entries->entries, dir_entries->ent_len, sizeof(struct EntryWithStat), compare_entries);
}

void dir_entries_print(struct DirEntries *dir_entries, struct Args args, int result_list_len) {
  if (args.flags.long_list_fmt) {
    if (result_list_len > 1) {
      printf("%s:\n", dir_entries->foldername);
    }
    printf("total: %llu\n", dir_entries->total_blocks);
  }

  for (int j = 0; j < dir_entries->ent_len; j++) {
    struct EntryWithStat entry_with_stat = dir_entries->entries[j];
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

  if (!args.flags.long_list_fmt) {
    printf("\n");
  }
}

void dir_entries_free(struct DirEntries *dir_entries) {
  for (int j = 0; j < dir_entries->ent_len; j++) {
    struct EntryWithStat entry_with_stat = dir_entries->entries[j];
    struct LocalEntry *entry = entry_with_stat.entry;
    struct stat *stat = entry_with_stat.stat;

    free(entry->d_name);
    free(entry);
    free(stat);
  }

  // // no need to free because this is coming from argv
  // // free(): invalid pointer
  // free(dir_entries->foldername);
  free(dir_entries->entries);
  free(dir_entries);
}
