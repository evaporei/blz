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
- [x] add minimal ~~Makefile~~ meson + ninja to build the project
- [ ] add color support

To compile:

```bash
meson setup build
ninja -C build
```

To run:

```bash
./build/blz -l -a folder1 fileA ...
```
