# blz

Better Ls and Zicaaaaa.

For computer science class.

- [x] add proper suffix for each file type (`*`, `/`, `@`, `|`, `=`)
- [x] add support for multiple folders (`foo: ...\n bar: ...`)
- [x] show dot files (`-a`)
- `-l`
  - [x] show total number of blocks for the directory
  - [x] show all data about each file found (permissions, links, owner, group, size, last modified and name)
- [x] print files in alphabetic order
- [x] organize/refactor code
- [ ] add minimal Makefile to build the project
- [ ] add color support

To compile (it may be outdated):

```bash
cc -g args.c dir_entries.c entry_with_stat.c error.c result_list.c string.c main.c
```

To run:

```bash
./a.out -l -a folder1 fileA ...
```
