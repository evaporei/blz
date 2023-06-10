#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "args.h"
#include "dir_entries.h"
#include "entry_with_stat.h"
#include "error.h"
#include "result_list.h"

int main(int argc, char* argv[]) {
  struct Args args = parse_args(argc, argv);

  struct ResultList results = result_list_new();

  // get the data using sys/lib calls
  for (int i = 0; i < args.folders_len; i++) {
    result_list_grow(&results);

    if (access(args.foldernames[i], F_OK) != 0) {
      struct Error *err = error_new(args.foldernames[i]);

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

      struct EntryWithStat entry_with_stat = entry_with_stat_new(args.foldernames[i], entry);

      dir_entries_append(dir_entries, entry_with_stat);
    }

    dir_entries_sort(dir_entries);

    result_list_append_dir_entries(&results, dir_entries);

    int close_res = closedir(dir);

    if (close_res < 0) {
      perror("blz: failed to close directory (closedir)");
    }
  }

  result_list_print(results, args);

  result_list_free(results);
}
