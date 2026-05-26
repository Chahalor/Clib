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

- setup setup the requested modules in the targeted dir
- update update the locals modules by updating the sources files and and importing new dependencies.
- export take the targeted dir and export at as a lib dir (create the lib files)

### Files

#### export
this file will say which module exist, where are they situated and which are they dependencies.


legend:
` (does not count in the format, only for the README representation)
 * mandatory informatiom
 - optional information
`
format
```
# comment
/* same here */
LIB NAME

[Module Name]
* headerFile: path
* dependencies: [
	module name,
	module name, module name
]
- Controls: path


```

#### import