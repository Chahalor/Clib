# Clib: TODO

## interface
### args
 - [ ] this module DA should become CLI
 - [ ] add controls with controller

## files
### JSON
 - [] empty array/object should represented has closed bracer/braket ([] and {})
 
### filesystem
this module would:
 - functions/types to manage path easly
 - functions do create file/dir based on a path (not juste one file/dir but the full tree)
 - functions to delete file/dir based on a path (not juste one file/dir but the full tree)
 - functions to copy   file/dir based on a path (not juste one file/dir but the full tree)
 - 
### toml
- [ ] nested named array are not counted as array but has string (so two or more nested array are lost)
```toml
array = [
	d = [
		bob,
		f = [tkt]
	],
	hello
]
> array = ["d = [\nbob", "f = [tkt]\n]", "hello"]
```

- [ ] multiple array without variable name lose they data
```toml
array = [
	[
		bob,
		[tkt]
	],
	hello
]
> array = [[], [], "hello"]
```

- [ ] toml error display is too long and does not display correcly error appening in multi line array
```toml
dependencies = [
	memory,
	"
]
```
output
```
┌┤Error: Unterminated TOML string (dev/config.toml:9:16)
│ dependencies
│                ─┬─
└─────────────────┘
```