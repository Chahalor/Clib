# filesystem

Goal:
Provide a unified API to manipulate files, directories, paths and filesystem resources without having to deal directly with low-level POSIX calls.

---

## Path management

### Path manipulation

- path join
- path split
- path dirname
- path basename
- path extension
- path filename
- path normalize (`/foo//bar/../baz`)
- path absolute
- path relative
- path realpath
- path compare
- path is absolute
- path is relative

### Types

- filesystem path type
- filesystem path iterator

---

## File / Directory informations

### Existence

- file exists
- directory exists
- path exists

### Type detection

- is file
- is directory
- is symlink
- is socket
- is fifo
- is block device
- is character device

### Metadata

- size
- permissions
- owner uid
- owner gid
- creation time
- modification time
- access time

---

## Creation

### Files

- create file
- create empty file
- create temporary file

### Directories

- create directory
- create directory tree (`mkdir -p`)
- create temporary directory

---

## Deletion

### Files

- remove file

### Directories

- remove empty directory
- remove directory tree (`rm -rf`)

### Generic

- remove path

---

## Copy

### Files

- copy file

### Directories

- copy directory
- copy directory tree

### Options

- overwrite destination
- preserve permissions
- preserve ownership
- preserve timestamps
- follow symlinks
- do not follow symlinks

---

## Move / Rename

### Filesystem objects

- move file
- move directory
- rename path
- move tree

### Cross-filesystem support

- fallback to copy + delete when rename fails

---

## Read

### Files

- read file
- read file into buffer
- read file lines
- read partial file
- read binary file

### Directory

- list directory
- iterate directory entries

---

## Write

### Files

- write file
- append file
- truncate file

### Safe writes

- atomic write
- atomic replace

---

## Links

### Symbolic links

- create symlink
- read symlink

### Hard links

- create hard link

---

## Permissions

### Access

- chmod
- chown
- change group
- change umask

### Checks

- readable
- writable
- executable

---

## Traversal

### Iteration

- iterate directory

### Recursive walk

- walk filesystem tree
- walk files only
- walk directories only

### Filters

- by extension
- by name
- by type

---

## Search

### Generic search

- find path by name
- find path by extension
- find path by pattern
- find path recursively

### Advanced

- find by size
- find by modification date
- find by permissions

---

## Comparison

### Files

- compare content
- compare metadata

### Trees

- compare directory trees

---

## Hash

### File hashes

- MD5
- SHA1
- SHA256
- SHA512

### Utilities

- compare hashes
- hash entire directory tree

---

## Disk informations

### Filesystem

- total space
- free space
- used space

### Mount points

- list mount points
- get filesystem type

---

## Monitoring

### Watchers

- watch file creation
- watch file deletion
- watch file modification
- watch directory changes

### Linux backend

- inotify

---

## Archives (optional)

### Create

- create tar archive

### Extract

- extract tar archive

### Inspect

- list archive content

---

## Utilities

### Temporary resources

- create temporary file
- create temporary directory

### Synchronization

- flush file
- fsync file

### Validation

- validate path
- validate filename

### Conversion

- file size to human readable string
- permission string conversion

---

## Error handling

### Generic

- filesystem error codes
- filesystem error strings

### Helpers

- convert errno to filesystem error

---

## Future extensions

- mmap helpers
- file locking
- async filesystem operations
- filesystem cache
- virtual filesystem layer (VFS)
- remote filesystem support (ssh, ftp, sftp)
- archive-as-filesystem support

## achitecture

filesystem/
├── path/
├── file/
├── directory/
├── walk/
├── search/
├── watch/
├── hash/
├── disk/
└── archive/