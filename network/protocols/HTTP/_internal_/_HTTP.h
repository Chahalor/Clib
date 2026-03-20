/**
 * @file _HTTP.h
 * @brief Internal HTTP helpers shared by parsing, serialization, and memory code.
 */

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <ctype.h>

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "_types.h"
# include "../settings.h"
# include "../error.h"

/* -----| Modules   |----- */
# include "../../../../memory.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

extern struct _s_http_allocators	_g_net_prot_http_allocator;

/* ************************************************************************** */
/*                           File-Local Helpers                               */
/* ************************************************************************** */

/**
 * @brief (`_memory.c`, static) Hash a normalized header key with 32-bit FNV-1a.
 * @param key NUL-terminated normalized key.
 * @return Hash value used for header-bucket indexing.
 * @par Issues
 * Depends on caller normalization; no NULL guard.
 */

/**
 * @brief (`_methods.c`, static) Compare method token candidate and expected token.
 * @param s1 Candidate string.
 * @param s2 Expected token.
 * @return Intended as token-match check for `s1` prefix.
 * @par Issues
 * Function is currently unused and implemented with inverted/invalid boolean logic.
 */

/**
 * @brief (`_parser.c`, static) Parse CRLF header lines into a header list.
 * @param list Output header list.
 * @param raw Header block start.
 * @return Offset to the blank-line terminator, or `-1` on parse/allocation failure.
 * @par Issues
 * Uses `strlen(raw)` on sized input, so it may read past parser buffer bounds.
 */

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

#pragma region Memory

/**
 * @brief Lowercase a NUL-terminated string in place.
 * @param str Mutable string to normalize.
 * @note Used to normalize header keys before hashing.
 * @par Issues
 * No NULL guard.
 * `tolower` is called with `char` directly, which is undefined for negative values.
 */
static inline void	_lower(
	char *str
)
{
	while (*str)
	{
		*str = tolower(*str);
		str++;
	}
}

/**
 * @brief Allocate and initialize an HTTP request container.
 * @param version Encoded HTTP version.
 * @param method HTTP method enum.
 * @param headers Header collection moved into the returned object.
 * @param body Body descriptor moved into the returned object.
 * @return Allocated request object, or `NULL` on allocation failure.
 * @note On allocation failure, `g_net_prot_http_settings.errno` is set.
 * @par Issues
 * Implementation currently uses signature
 * `_http_new(version, method, path, headers_ptr, body_ptr)` in `_memory.c`,
 * but this declaration does not expose `path` nor pointer arguments.
 */
t_http	*_http_new(
	const float version,
	const t_http_methods method,
	const t_http_header_list *const headers,
	const t_http_body *const body
);

/**
 * @brief Allocate a header node and copy key/value strings.
 * @param key Header key (stored normalized to lowercase).
 * @param value Header value.
 * @return New header node, or `NULL` on allocation failure.
 * @note On failure, `g_net_prot_http_settings.errno` is set to `HTTP_ERROR_SYSCALL`.
 * @par Issues
 * Implementation allocates `strlen(value)` bytes (missing `+1`) before `strcpy`,
 * which can overflow.
 */
t_http_header	*_http_new_header(
	const char *const key,
	const char *const value
);

/**
 * @brief Allocate the hash bucket array for a header list.
 * @param list Target list to initialize.
 * @param capacity Number of hash buckets.
 * @return `error_none` on success, `error_alloc_fail` on allocation failure.
 * @note On allocation failure, `g_net_prot_http_settings.errno` is set to `HTTP_ERROR_SYSCALL`.
 * @par Issues
 * Bucket memory is not zero-initialized and `list->size` is not reset here.
 * `capacity <= 0` is not rejected.
 */
int	_http_setup_header_list(
	t_http_header_list *const list,
	const int capacity
);

/**
 * @brief Insert a header into both hash chain and insertion-order list.
 * @param list Destination list.
 * @param header Header node to append.
 * @return Always `error_none` in current implementation.
 * @note Insertion order is tracked via `order_prev/order_next`.
 * @par Issues
 * No NULL/capacity validation and no duplicate-key handling.
 */
int	_http_add_header(
	t_http_header_list *const list,
	t_http_header *const header
);

/**
 * @brief Remove the first header matching `key`.
 * @param list Header list.
 * @param key Header key to remove.
 * @param free When non-zero, free the removed node.
 * @note Updates both hash-chain links and insertion-order links.
 * @par Issues
 * Matching is exact and case-sensitive; callers must pass normalized keys.
 * No NULL/capacity validation.
 */
void	_http_remove_header(
	t_http_header_list *const list,
	const char *const key,
	const int free
);

/**
 * @brief Find a header by key in the hash map.
 * @param list Header list.
 * @param key Header key (expected normalized).
 * @return Matching header pointer, or `NULL` if not found.
 * @par Issues
 * No NULL/capacity validation.
 * Matching is exact and does not normalize input.
 */
t_http_header	*_http_header_find(
	t_http_header_list *const list,
	const char *const key
);

/**
 * @brief Free an HTTP object.
 * @param target Object to free.
 * @param all When non-zero, also free the header-list internals recursively.
 * @par Issues
 * Body payload memory is not released here.
 * No NULL guard.
 */
void	_http_free(
	t_http *const target,
	const int all
);

/**
 * @brief Free one header node.
 * @param header Header to free.
 * @param recursive When non-zero, recursively free `header->next`.
 * @note Recursion follows hash-chain `next`, not insertion-order links.
 * @par Issues
 * No NULL guard.
 */
void	_http_free_header(
	t_http_header *const header,
	const int recursive
);

/**
 * @brief Free a header-list bucket array and optionally all headers.
 * @param list List to release.
 * @param recursive When non-zero, free headers in each bucket.
 * @par Issues
 * Current recursive path can double-free/use-after-free because both
 * `_http_free_list` and `_http_free_header` walk `next`.
 * With `recursive == 0`, headers are leaked.
 * Fields are not reset after free.
 */
void	_http_free_list(
	t_http_header_list *const list,
	const int recursive
);

#pragma region Methods

/**
 * @brief Convert method enum to its request-token string.
 * @param method Method enum value.
 * @return Pointer to static string table entry.
 * @par Issues
 * No bounds validation on `method`.
 * Current conditional initializer can leave some entries unset for versions >= 1.1.
 */
const char	*_http_methods_str(
	const t_http_methods method
);

/**
 * @brief Parse an uppercase HTTP method token.
 * @param str Method token string.
 * @return Parsed enum, or `HTTP_ERROR` when unsupported/invalid.
 * @par Issues
 * Comparison is strict (`strcmp`) and does not accept trailing spaces.
 * `PUT` cannot match because it is checked inside the `'P'` switch branch.
 */
t_http_methods	_http_str_to_method(
	const char *str
);

#pragma region Parsing

/**
 * @brief Read the next CRLF-terminated line from a bounded buffer.
 * @param buf Source buffer.
 * @param len Buffer length.
 * @param _offset In/out read offset.
 * @param _line Out pointer to line start (inside `buf`).
 * @param _line_len Out line length (without CRLF).
 * @return `1` when a line is found, `0` when no full CRLF line remains.
 * @par Issues
 * No NULL-argument validation.
 */
int	_next_line(
	const char *const buf,
	const size_t len,
	size_t *_offset,
	const char **_line,
	size_t *const _line_len
);

/**
 * @brief Parse a signed decimal integer prefix.
 * @param s Input string.
 * @param end Out pointer to the first non-digit after the parsed prefix.
 * @return Parsed `ssize_t` value.
 * @par Issues
 * No overflow checks.
 * Empty numeric part returns `0`, which is ambiguous with valid `"0"`.
 */
ssize_t	_str_to_nb(
	const char *const s,
	const char **end
);

/**
 * @brief Parse an HTTP version token (`HTTP/<major>.<minor>`).
 * @param str Version token string.
 * @return Encoded version, or `0` on parse failure.
 * @par Issues
 * Trailing CRLF validation condition is incorrect and may accept malformed tails.
 * No overflow/range checks on parsed digits.
 */
t_http_version	_http_parse_version(
	const char *str
);

/**
 * @brief Format an encoded version into `HTTP/x.y`.
 * @param v Encoded version.
 * @param buf Destination buffer.
 * @return `buf` on success, `NULL` when `buf == NULL`.
 * @note Caller must provide enough space (at least 9 bytes).
 * @par Issues
 * Only supports single-digit major/minor values (`'0' + digit` logic).
 */
char	*_http_version_to_str(
	t_http_version v,
	char *const buf
);

/**
 * @brief Parse an HTTP/1.0-style request from raw bytes.
 * @param raw Raw request buffer.
 * @param size Buffer size.
 * @param error Out HTTP status code on failure.
 * @return Parsed request object, or `NULL` on error.
 * @note On failure, this function sets `*error` and may set `g_net_prot_http_settings.errno`.
 * @par Issues
 * Method parsing calls `_http_str_to_method(raw)` on the full request line, which
 * makes valid requests fail with the current strict method parser.
 * `_memory.c` implementation of `_http_new` requires a `path`, but this call path
 * does not provide one.
 * Header parsing uses `strlen` on a sized buffer, which can read past `size`.
 * Several failure paths leak previously allocated memory.
 */
t_http	*_http_parse_1_0(
	const char *const raw,
	const ssize_t size,
	int *const error
);

#pragma region Strings

/**
 * @brief Serialize a request object into raw HTTP bytes.
 * @param http Source request.
 * @param buff Destination buffer, optional.
 * @param size Buffer size; when `buff == NULL && size > 0`, a new fixed-size buffer is allocated;
 *             when `size == 0`, a dynamic buffer is allocated and grown by 1024-byte chunks.
 * @return Pointer to serialized bytes, or `NULL` on failure/insufficient space.
 * @note Adds `Content-Length` automatically when `content_length > 0` and header is missing.
 * @par Issues
 * Fixed-size mode returns `NULL` on overflow without reporting required size.
 * Dynamic realloc path can leak memory on realloc failure (pointer overwritten directly).
 * No NULL guards for `http`/critical subfields.
 */
char	*_http_request_raw(
	const t_http *const http,
	char *const buff,
	size_t size
);
