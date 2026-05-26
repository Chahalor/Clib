/**
 * @file toml.h
 * @brief Public API for the TOML module.
 */
#ifndef CLIB_FILES_TOML_H
# define CLIB_FILES_TOML_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
# include <stdio.h>
# include <stdarg.h>
# include <stdint.h>

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "../standards/lib_standards.h"

/* -----| Internals |----- */
# include "toml/types.h"
# include "toml/_internal_/_types.h"

/* -----| Modules   |----- */
# include "../memory.h"
# include "../processor/processor.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

/**
 * @brief Iterate over the direct children of a TOML table or array node.
 *
 * `toml_foreach()` creates a loop variable named by `var` and advances through
 * the `next` links of `node`'s children. Passing `NULL` is allowed and produces
 * an empty loop.
 *
 * @param var Name of the loop-local `TOML *` variable to create.
 * @param node Table or array node whose direct children should be visited.
 *
 * @example
 * @code
 * TOML *arr = (TOML *)toml_get(doc, "numbers");
 *
 * toml_foreach(item, arr)
 * {
 *     printf("type=%d\n", toml_get_type(item));
 * }
 * @endcode
 */
# define toml_foreach(var, node)												\
	for (TOML *(var) = ((TOML *)(node) ? ((TOML *)(node))->child : NULL);	\
		(var) != NULL;														\
		(var) = (var)->next)

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/**
 * @brief Allocate a new empty TOML document.
 *
 * The returned document is a table root and can receive fields with
 * `toml_set()`, `toml_set_wild()`, or the other setter helpers. Release it with
 * `toml_unload()` or `toml_free()`.
 *
 * @return New TOML root node, or `NULL` on allocation failure.
 *
 * @example
 * @code
 * TOML *doc = toml_new();
 * toml_set(doc, "project.name", "clib", toml_str);
 * toml_unload(doc);
 * @endcode
 */
TOML	*toml_new(void);

/**
 * @brief Load and parse a TOML document from a file path.
 *
 * The file content is parsed into a new TOML root node. The returned object is
 * owned by the caller and must be released with `toml_unload()` or
 * `toml_free()`.
 *
 * @param path Path to the TOML file to read. Must not be `NULL`.
 *
 * @return Parsed TOML root node, or `NULL` on invalid input, file error,
 * allocation failure, or parse failure.
 *
 * @example
 * @code
 * TOML *cfg = toml_load_file("config.toml");
 * const char *name = toml_get(cfg, "package.name");
 * toml_unload(cfg);
 * @endcode
 */
TOML	*toml_load_file(
				const char *const restrict path
				);

/**
 * @brief Build and parse a TOML document from a formatted string.
 *
 * `format` is expanded by the module formatter, then parsed as TOML text. The
 * returned root is owned by the caller and must be released.
 *
 * @param format Format string containing TOML content. Must not be `NULL`.
 * @param ... Values consumed by the format string.
 *
 * @return Parsed TOML root node, or `NULL` on invalid input, allocation
 * failure, or parse failure.
 *
 * @example
 * @code
 * TOML *doc = toml_load_str("name = \"%s\"\nage = %d\n", "alice", 42);
 * printf("%s\n", (char *)toml_get(doc, "name"));
 * toml_unload(doc);
 * @endcode
 */
TOML	*toml_load_str(
				const char *const restrict format,
				...
				);

/**
 * @brief Free a TOML document tree.
 *
 * This releases the passed node and all of its children. It is intended for root
 * documents returned by `toml_new()`, `toml_load_file()`, and `toml_load_str()`.
 *
 * @param toml TOML node tree to free. Must not be `NULL`.
 *
 * @return `error_none` on success, or `-error_invalid_arg` when `toml` is
 * `NULL`.
 *
 * @example
 * @code
 * TOML *doc = toml_new();
 * int err = toml_unload(doc);
 * @endcode
 */
int		toml_unload(
				TOML *toml
				);

/**
 * @brief Free a TOML node tree when non-NULL.
 *
 * This is a void-returning convenience wrapper around the same recursive free
 * behavior used by `toml_unload()`. Passing `NULL` is allowed.
 *
 * @param obj TOML node tree to free, or `NULL`.
 *
 * @example
 * @code
 * TOML *doc = toml_load_str("enabled = true\n");
 * toml_free(doc);
 * @endcode
 */
void	toml_free(
				TOML *const restrict obj
				);

/**
 * @brief Get a value or child node by dotted path.
 *
 * For scalar nodes, the return value is the stored `char *` data. For table and
 * array nodes, the return value is a borrowed `TOML *`. The path may contain
 * formatting placeholders consumed by the variadic arguments.
 *
 * @param obj Root or parent node to search from. Must not be `NULL`.
 * @param key Dotted path to the target field. Must not be `NULL`.
 * @param ... Optional values consumed by `key` formatting.
 *
 * @return Scalar `char *`, container `TOML *`, or `NULL` when the field is not
 * found or formatting fails.
 *
 * @example
 * @code
 * const char *name = toml_get(doc, "package.name");
 * const char *user = toml_get(doc, "users.%s.name", "alice");
 * TOML *items = (TOML *)toml_get(doc, "package.items");
 * @endcode
 */
void	*toml_get(
				TOML *const obj,
				const char *const restrict key,
				...
				);

/**
 * @brief Return the approximate allocated size of a TOML tree.
 *
 * The count includes node structures and duplicated key/data strings reachable
 * from `toml`.
 *
 * @param toml TOML node tree to measure.
 *
 * @return Size in bytes, or `0` when `toml` is `NULL`.
 *
 * @example
 * @code
 * size_t bytes = toml_sizeof(doc);
 * printf("document uses about %zu bytes\n", bytes);
 * @endcode
 */
size_t	toml_sizeof(
				const TOML *const restrict toml
				);

/**
 * @brief Count direct children of a TOML table or array node.
 *
 * Scalars have no child length and return `0`.
 *
 * @param toml Table or array node to inspect.
 *
 * @return Number of direct children, or `0` for `NULL` and scalar nodes.
 *
 * @example
 * @code
 * TOML *arr = (TOML *)toml_get(doc, "numbers");
 * printf("numbers length: %zu\n", toml_len(arr));
 * @endcode
 */
size_t	toml_len(
				const TOML *const restrict toml
				);

	/**
	 * @brief Set a field using compile-time dispatch for common value types.
	 *
	 * Numeric values are routed to number setters, string values are routed to
	 * `toml_set_string()`, and other values are routed to `toml_set_wild()`.
	 * `field` can be a dotted path and may contain formatting placeholders.
	 *
	 * @param toml TOML document or table to mutate.
	 * @param field Dotted target path to create or replace.
	 * @param var Value to write.
	 * @param ... Optional field-format arguments or type information required by
	 * non-string values.
	 *
	 * @return `error_none` on success, otherwise an error code.
	 *
	 * @example
	 * @code
	 * int age = 42;
	 * toml_set(doc, "user.name", "alice", toml_str);
	 * toml_set(doc, "user.age", age);
	 * toml_set(doc, "users.%s.name", "bob", toml_str, "admin");
	 * @endcode
	*/
#define toml_set(toml, field, var, ...) \
		__builtin_choose_expr( \
		_is_number(var), \
		__builtin_choose_expr( \
			_is_signed(var), \
			toml_set_signed_nb, \
			toml_set_unsigned_nb \
		), \
		__builtin_choose_expr( \
			_is_string(var), \
			toml_set_string, \
			toml_set_wild \
		) \
		)(toml, field, var, ##__VA_ARGS__)

/**
 * @brief Set a signed integer field.
 *
 * The integer is converted to decimal text and stored at `field`, creating
 * intermediate tables as needed.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param var Signed integer value to store.
 * @param ... Optional values consumed by `field` formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * toml_set_signed_nb(doc, "server.port", 8080);
 * toml_set_signed_nb(doc, "users.%s.id", -10, "guest");
 * @endcode
 */
int	toml_set_signed_nb(
		TOML *const toml,
		const char *const field,
		signed long long var, ...
	);

/**
 * @brief Set an unsigned integer field.
 *
 * The integer is converted to decimal text and stored at `field`, creating
 * intermediate tables as needed.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param var Unsigned integer value to store.
 * @param ... Optional values consumed by `field` formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * toml_set_unsigned_nb(doc, "limits.max_connections", 1024u);
 * toml_set_unsigned_nb(doc, "users.%s.uid", 1000u, "alice");
 * @endcode
 */
int	toml_set_unsigned_nb(
		TOML *const toml,
		const char *const field,
		unsigned long long var, ...
	);

/**
 * @brief Set a string field.
 *
 * `var` is formatted using the variadic arguments and stored as a TOML string at
 * `field`.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param var String or format string to store. Must not be `NULL`.
 * @param ... Optional values consumed by `field` and `var` formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * toml_set_string(doc, "package.name", "clib");
 * toml_set_string(doc, "package.version", "%d.%d.%d", 1, 2, 0);
 * @endcode
 */
int	toml_set_string(
		TOML *const toml,
		const char *const field,
		char *var, ...
	);

/**
 * @brief Set a table field from another TOML table node.
 *
 * The source table's children are cloned into the target field. The caller keeps
 * ownership of `var`.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param var Source table node, or `NULL` to create an empty table.
 * @param ... Optional values consumed by `field` formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * TOML *profile = toml_new();
 * toml_set(profile, "name", "alice", toml_str);
 * toml_set_obj(doc, "users.alice", profile);
 * toml_unload(profile);
 * @endcode
 */
int	toml_set_obj(
		TOML *const toml,
		const char *const field,
		TOML *var, ...
	);

/**
 * @brief Set an array field from another TOML array node.
 *
 * The source array's children are cloned into the target field. The caller keeps
 * ownership of `var`.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param var Source array node, or `NULL` to create an empty array.
 * @param ... Optional values consumed by `field` formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * TOML *src = toml_load_str("items = [1, 2, 3]\n");
 * toml_set_array(doc, "copy", toml_get(src, "items"));
 * toml_unload(src);
 * @endcode
 */
int	toml_set_array(
		TOML *const toml,
		const char *const field,
		void *var, ...
	);

/**
 * @brief Set a field by explicitly providing the TOML type.
 *
 * Use this when the generic `toml_set()` macro cannot infer the intended type,
 * or when storing booleans, floats, datetimes, arrays, tables, or null values.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param var Pointer or string value matching `type`.
 * @param type One of `t_toml_type`.
 * @param ... Optional values consumed by `field` formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * int enabled = 1;
 * toml_set_wild(doc, "server.enabled", &enabled, toml_bool);
 * toml_set_wild(doc, "server.ratio", "0.75", toml_float);
 * toml_set_wild(doc, "released", "2026-05-26T10:00:00Z", toml_datetime);
 * @endcode
 */
int	toml_set_wild(
		TOML *const toml,
		const char *const field,
	void *var,
		int type, ...
	);

/**
 * @brief Create an array field from a C array.
 *
 * Each C array entry is appended to a new TOML array using the same `type`.
 * Intermediate tables in `field` are created as needed.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param array Source C array. May be `NULL` only for zero-length or null arrays.
 * @param length Number of entries in `array`.
 * @param type TOML type to use for each entry.
 * @param ... Optional values consumed by `field` formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * int nums[] = {1, 2, 3};
 * char *names[] = {"alice", "bob"};
 * toml_set_from_array(doc, "numbers", nums, 3, toml_int);
 * toml_set_from_array(doc, "users.names", names, 2, toml_str);
 * @endcode
 */
int		toml_set_from_array(
				TOML *toml,
				const char *const restrict field,
			void *const array,
			const size_t length,
			const int type,
				...
				);

/**
 * @brief Append one value to a TOML array node.
 *
 * `toml` must be an existing array node, usually retrieved with `toml_get()` or
 * created by `toml_set_from_array()`.
 *
 * @param toml Array node to append to. Must not be `NULL`.
 * @param value Pointer or string value matching `type`.
 * @param type TOML type of `value`.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * int value = 4;
 * TOML *arr = (TOML *)toml_get(doc, "numbers");
 * toml_array_append(arr, &value, toml_int);
 * toml_array_append(arr, "done", toml_str);
 * @endcode
 */
int		toml_array_append(
				TOML *const toml,
				void *const value,
				const int type
				);

/**
 * @brief Remove one value from a TOML array node by index.
 *
 * Remaining entries keep their relative order.
 *
 * @param toml Array node to mutate. Must not be `NULL`.
 * @param index Zero-based index of the entry to remove.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * TOML *arr = (TOML *)toml_get(doc, "numbers");
 * toml_array_pop(arr, 0);
 * @endcode
 */
int		toml_array_pop(
				TOML *const toml,
				const size_t index
				);

/**
 * @brief Detach a field from a TOML document without freeing the removed node.
 *
 * This removes the node from its parent. Use `toml_remove()` when the removed
 * node should be freed immediately.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param ... Optional values consumed by `field` formatting.
 *
 * @return `error_none` on success, including when the field is absent;
 * otherwise an error code.
 *
 * @example
 * @code
 * toml_unset(doc, "cache.temporary");
 * toml_unset(doc, "users.%s.session", "alice");
 * @endcode
 */
int		toml_unset(
				TOML *toml,
				const char *const restrict field,
				...
				);

/**
 * @brief Remove and free a field from a TOML document.
 *
 * The removed node and its children are freed. Missing fields are treated as a
 * successful no-op.
 *
 * @param toml TOML document or table to mutate. Must not be `NULL`.
 * @param field Dotted target path. Must not be `NULL`.
 * @param ... Optional values consumed by `field` formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * toml_remove(doc, "users.alice.password");
 * toml_remove(doc, "users.%s.password", "bob");
 * @endcode
 */
int		toml_remove(
				TOML *toml,
				const char *const restrict field,
				...
				);

/**
 * @brief Write a TOML document to an open stream.
 *
 * The document is serialized and written to `file`. `pretty` controls spacing in
 * generated arrays and tables; pass `0` for compact output.
 *
 * @param toml TOML document or node to dump. Must not be `NULL`.
 * @param file Open `FILE *` stream to write to. Must not be `NULL`.
 * @param pretty Number of spaces used for readable formatting.
 *
 * @return `error_none` on success, otherwise an error code.
 *
 * @example
 * @code
 * toml_dump(doc, stdout, 2);
 * FILE *out = fopen("config.toml", "w");
 * toml_dump(doc, out, 0);
 * fclose(out);
 * @endcode
 */
int		toml_dump(
				TOML *const restrict toml,
				FILE *const file,
				const int pretty
				);

/**
 * @brief Serialize a TOML document or node to a newly allocated string.
 *
 * The caller owns the returned string and must free it with the module allocator
 * (`g_toml_settings.free`) or the matching memory helper used by the project.
 *
 * @param toml TOML document or node to serialize. Must not be `NULL`.
 * @param pretty Number of spaces used for readable formatting; `0` is compact.
 *
 * @return Newly allocated TOML text, or `NULL` on invalid input or allocation
 * failure.
 *
 * @example
 * @code
 * char *text = toml_stringify(doc, 2);
 * puts(text);
 * g_toml_settings.free(text);
 * @endcode
 */
char	*toml_stringify(
				TOML *toml,
				const int pretty
				);

/**
 * @brief Check whether a TOML node has a given type.
 *
 * @param node Node to inspect. May be `NULL`.
 * @param type Expected TOML type.
 *
 * @return `1` when `node` is non-NULL and has `type`, otherwise `0`.
 *
 * @example
 * @code
 * TOML *arr = (TOML *)toml_get(doc, "numbers");
 * if (toml_assert_type(arr, toml_array))
 *     printf("numbers is an array\n");
 * @endcode
 */
int		toml_assert_type(
				const TOML *const node,
				const int type
				);

/**
 * @brief Return the type of a TOML node.
 *
 * @param node Node to inspect. May be `NULL`.
 *
 * @return One of `t_toml_type`, or `0` when `node` is `NULL`.
 *
 * @example
 * @code
 * void *value = toml_get(doc, "package");
 * int type = toml_get_type((TOML *)value);
 * @endcode
 */
int		toml_get_type(
				const TOML *const node
				);

#endif	// CLIB_FILES_TOML_H
