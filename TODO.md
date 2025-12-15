# Clib: TODO

### memory
- [X] better memory management, like a tree or someting like that, not a linked list xD
- [X] add mem_dup() function
- [X] fix invalid read/write using the rb tree
- [X] fix `mem_realloc()`, it is the dawa.

### server
- [X] server setup
- [ ] communication protocol
	- [ ] connection
	- [ ] communication
	- [ ] deconnection
	- [ ] shutdown
	- [ ] timeout
- [X] reconnection on lost connection management
- [X] timeout management
- [ ] anti flood management (aka on peut pas flood le manager/slaves/sentinel)
- [X] fix compilation

### interface

#### json
- [X] add a json_open that open the file
- [X] add a json_parse that parse the file
- [X] add a json_load that open and parse the file
- [X] add json_unload to free and close the JSON struct
- [X] add json_get(JSON, PATH) that will return the string/JSON of the field
- [X] add json_new() that will create a new JSON element
- [X] add json_dump(JSON, PATH, data, type) that will write data to the field PATH of the json
- [X] add json_dump(JSON, FILE, indent) that will write the JSON to the FILE
- [X] fix `json_dump()` it write nothing rigth (exept int)
- [X] empty `json_dump()` add a single '}' at the end
- [X] fix `json_unset()` that unset nothing
- [X] finish to test `json_unset()`
- [X] test all that shit

#### logs
- [X] change log_manager to va_args
- [X] add a log_perror that take from the errno
- [ ] add a log_error  that take from the lib internal errno
- [X] add a log_error with has a MACRo to add the \__func__ at the begining of _format 
- [X] add an option to add timestamp/date/time before the log
- [X] automatic '\n' at the end of the log
- [X] if log_init.filename, default mode are `O_CREAT | O_APPEND | O_RDWR` and perms `0644`

#### io
 - not for this version

## other

# Clib: TODO
## lib
- [ ] add an option to reconfigue config variable with global variable like errno (sigelton cacher)
- [ ] create a config module to manage all the lib
### interface
#### json
- [ ] change the data format for somethnig better, like a tree, no more linked list (red black tree ???)
- [ ] add a `json_getf()` that get the argument based on a format (printf format)

## other
- [ ] add a `CLI` option to check that ne thing does not break every things