# Lib manager

## Goal:
Only copy the sources files for the needed modules of the lib and they dependencies

---
exemples:
```bash
> ls
build           files.h      lib.h    memory     processor  review.md  times.h
check.sh        interface    Manager  memory.h   random     standards  TODO.md
CMakeLists.txt  interface.h  math     network    random.h   tests
files           _internal_   math.h   network.h  README.md  times
```

if i only want the memory and the json module, only thos two modules will be copyed.
If i would only want the json module all of it dependencies would also be copied (like the memory one).

## usage

### CLI
libmng setup/update/export

- init:		init the lib in the current session
- setup:	setup the requested modules in the targeted dir
- update	update the locals modules by updating the sources files and and importing new dependencies.
- list:		list all the available modules and the current version of tghe lib register in cache
- help:		same has --help/-h

### Files

#### export
this file will say which module exist, where are they situated and which are they dependencies.

format
```toml
name = "name"
version = "version"
url = "url to the repo"

[export]
includeDir = "include"
sourceDir = "src"
buildSystem = "cmake"

[[moduleName]]
path = "path/to/the/module"
publicHeaders = "header.h"
privateHeaders = "_internal_/_header.h"
dependencies = []
defines = [
	"MEMORY_DEBUG"
]
tags = [
	"core",
	"allocator"
]
controls = [
	"path/to/the/controls/"
]
```

exemple:
```toml
[[json]]
path = "files/json/"
publicHeaders = [
	"../json.h"
]
privateHeaders = [
	"_internal_/_json.h"
]

dependencies = [
	"memory"
]
```

#### import
```toml
modules = [
	module1,
	module2,
	module3,
	module4
]

[[moduleName]]
version=version
```