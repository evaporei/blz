#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cli.h"
#include "dir_entries.h"
#include "entry_with_stat.h"
#include "result_list.h"
#include "string.h"
#include "types.h"

int compare_entries(const void *a, const void *b) {
  struct EntryWithStat *entry_a = (struct EntryWithStat *) a;
  struct EntryWithStat *entry_b = (struct EntryWithStat *) b;
  return strcmp(entry_a->entry->d_name, entry_b->entry->d_name);
}

int main(int argc, char* argv[]) {
  struct Args args = parse_args(argc, argv);

  struct ResultList results = result_list_new();

  // get the data using sys/lib calls
  for (int i = 0; i < args.folders_len; i++) {
    result_list_grow(&results);

    if (access(args.foldernames[i], F_OK) != 0) {
      char *msg_template = "blz: cannot access '%s': No such file or directory";
      char *msg = malloc((strlen(msg_template) + strlen(args.foldernames[i]) - 1) * sizeof(char));
      sprintf(msg, msg_template, args.foldernames[i]);

      struct Error *err = malloc(sizeof(struct Error));
      err->msg = msg;
      err->kind = NoEntity;

      result_list_append_err(&results, err);

      continue;
    }

    DIR *dir = opendir(args.foldernames[i]);

    // it is actually a file
    if (dir == NULL) {
      result_list_append_filename(&results, args.foldernames[i]);
      continue;
    }

    struct DirEntries *dir_entries = dir_entries_new(args.foldernames[i]);

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
      dir_entries_grow(dir_entries);

      if (entry->d_name[0] == '.' && !args.flags.all) {
        continue;
      }

      struct EntryWithStat entry_with_stat;

      struct LocalEntry *local_entry = entry_new(entry);

      entry_with_stat.entry = local_entry;

      char *full_path = str_path_cat(args.foldernames[i], entry->d_name);

      struct stat *file_stat = stat_new(full_path);

      entry_with_stat.stat = file_stat;

      free(full_path);

      dir_entries_append(dir_entries, entry_with_stat);
    }

    // order/sort entries
    qsort(dir_entries->entries, dir_entries->ent_len, sizeof(struct EntryWithStat), compare_entries);

    result_list_append_dir_entries(&results, dir_entries);

    int close_res = closedir(dir);

    if (close_res < 0) {
      perror("blz: failed to close directory (closedir)");
    }
  }

  // print gathered data
  result_list_print(results, args);

  // free data
  result_list_free(results);
}
