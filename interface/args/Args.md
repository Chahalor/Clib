<!--
@file Args.md
@brief Overview and usage notes for the `interface/args` module.
-->

# interface/args

`interface/args` is a command-line parsing module.

It lets you:
- define root options and positional parameters
- define subcommands with their own options and parameters
- parse `argc`/`argv` into an output tree
- query parsed options/parameters with helper APIs

## Definition API

Main builder functions:
- `args_parser_new`
- `args_parser_add_option`
- `args_add_param` (`t_args_parser *` or `t_args_option *`)
- `args_parser_add_sub`
- `args_set_description`

Parameter flags:
- `args_param_specs_require`
- `args_param_specs_nargs`

## Parsing API

Use:
- `args_parse(parser, argc, argv)`
- `args_error(out)`
- `args_error_str(code)`

You can inspect presence with:
- `args_has_option`
- `args_has_param`
- `args_has_sub`
- `args_active_subcommand`
- `args_get_sub_output`

## Extraction API (current behavior)

### `args_get_option(var, name)`
`var` can be:
- `t_args_output *`
- `t_args_output_parser *`

Returns:
- `t_args_output_option *` if found
- `NULL` otherwise

### `args_get_param(var, name, &n)`
`var` can be:
- `t_args_output *`
- `t_args_output_parser *`

Returns:
- `NULL` when missing or empty (`n == 0`)
- `char *` when `n == 1`
- `char **` (NULL-terminated) when `n > 1`

Memory ownership:
- Return storage itself must be released with `mem_free`.
- For `char **`, only the returned array is owned; pointed strings belong to
  the parse output tree.

## Notes

- `args_show_help` currently exits the process after printing help.
- Parsed option/parameter references become invalid after `args_output_free`.
