/**
 * @file log.h
 * @brief Public API for the `interface/log` module.
 *
 * This header exposes configuration, typed logging entry points, and
 * convenience dispatch macros. The documentation below reflects the
 * current implementation behavior.
 * 
 * @author	@Chahalor
 * @date	30/03/2026
 */

#ifndef LIB_INTERFACE_LOG_H
# define LIB_INTERFACE_LOG_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "../standards/lib_standards.h"

/* -----| Internals |----- */
# include "log/types.h"

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

/**
 * @brief Size (in bytes) of the temporary buffer used for atomic log writes.
 *
 * Used by the internal `_logs_print_atomic()` path when assembling a complete
 * log record tree before writing to a file descriptor.
 *
 * @version 2.0.0
 *
 * @code{.c}
 * #include "interface/log.h"
 * @endcode
 */
# define LOG_BUFFER_SIZE 8192

/**
 * @brief Number of log levels handled by the module.
 *
 * The value matches the current `enum e_log_level` size:
 * `log_error`, `log_warning`, `log_info`, `log_debug`, `log_other`.
 *
 * @version 2.0.0
 */
# define	LOG_NB_LEVEL	5	/* number of availaible logs level */

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/**
 * @brief Type-dispatch logging entry point.
 *
 * Dispatch rules:
 * - If `var` is exactly `t_log_report *`, calls `logs_report(var)`.
 * - Otherwise calls `logs_raw(var, __func__, __FILE__, __LINE__, ...)`.
 *
 * @return Return code of the selected target function.
 * In current implementation the write path returns `error_none` (0).
 *
 * @version 2.0.0
 *
 * @code{.c}
 * // Raw call (auto-injects func/file/line)
 * logs(log_warning, 7, "validation warning", "missing optional field");
 *
 * // Report call
 * t_log_report rep = {
 *     .level = log_info,
 *     .summary = "startup",
 *     .body = "service ready",
 * };
 * logs(&rep);
 * @endcode
 */
# define	logs(var, ...) (								\
	__builtin_choose_expr(									\
		IS_TYPE(var, t_log_report *),						\
		_logs_report_adapter,								\
		logs_raw											\
	)(var, __func__, __FILE__, __LINE__, ## __VA_ARGS__)	\
)

/**
 * @brief Type-dispatch logging entry point with `errno` integration.
 *
 * Dispatch rules:
 * - If `var` is exactly `t_log_report *`, calls `logs_perror_report(var)`.
 * - Otherwise calls `logs_perror_raw(var, __func__, __FILE__, __LINE__, ...)`.
 *
 * The selected function appends `strerror(errno)` to the summary.
 *
 * @return Return code of the selected target function.
 * In current implementation the write path returns `error_none` (0).
 *
 * @version 2.0.0
 *
 * @code{.c}
 * // Raw call (auto-injects func/file/line, captures errno)
 * logs_perror(log_error, "open failed", "reading secrets.env");
 *
 * // Report call
 * t_log_report rep = {.level = log_error, .summary = "connect failed"};
 * logs_perror(&rep);
 * @endcode
 */
# define	logs_perror(var, ...) (							\
	__builtin_choose_expr(									\
		IS_TYPE(var, t_log_report *),						\
		_logs_perror_report_adapter,								\
		logs_perror_raw											\
	)(var, __func__, __FILE__, __LINE__, ## __VA_ARGS__)	\
)

/**
 * @brief Initialize the module-wide logging configuration singleton.
 *
 * Internally forwards to `_log_config(data)` which updates static state.
 *
 * @param	_data Configuration for each log stream and display behavior.
 *
 * @return `error_invalid_arg` when `_data == NULL`.
 * @return `1` when `_data != NULL` (current implementation behavior).
 *
 * @version 2.0.0
 *
 * @code{.c}
 * t_log_init init = {
 *     .error = {.filename = "error.log"},
 *     .display_limit = log_other,
 * };
 *
 * int rc = log_init(&init);
 * if (rc == error_invalid_arg) {
 *     // invalid pointer
 * }
 * @endcode
 */
int	log_init(
	const t_log_init *const _data
);

/**
 * @brief Log using explicit primitive fields.
 *
 * Builds a temporary `t_log_report` then forwards to internal `_logs(...)`.
 * Additional variadic arguments are currently ignored by the implementation.
 *
 * @param level		Log level.
 * @param func		Caller function name.
 * @param file		Caller file path.
 * @param line		Caller line number.
 * @param code		Application status/error code to print.
 * @param summary	Short one-line summary.
 * @param body		Optional multi-line body (split on `\n`).
 *
 * @return Current implementation returns `error_none` (0) on write path.
 *
 * @version 2.0.0
 *
 * @code{.c}
 * logs_raw(log_debug, __func__, __FILE__, __LINE__, 0, "cache warmup", "step1 ok\nstep2 ok");
 * @endcode
 */
int	logs_raw(
	const t_log_level level,
	const char *const func,
	const char *const file,
	const int line,
	const int code,
	const char *const summary,
	const char *const body, ...
);

/**
 * @brief Log using explicit primitive fields and append `strerror(errno)`.
 *
 * Builds a temporary `t_log_report` with `.code = errno`, then forwards to
 * internal `_logs(..., perr = true)`.
 * Additional variadic arguments are currently ignored by the implementation.
 *
 * @param level		Log level.
 * @param func		Caller function name.
 * @param file		Caller file path.
 * @param line		Caller line number.
 * @param summary	Short summary prefix (must be non-NULL in current code path).
 * @param body		Optional multi-line body.
 *
 * @return Current implementation returns `error_none` (0) on write path.
 *
 * @version 2.0.0
 *
 * @code{.c}
 * logs_perror_raw(log_error, __func__, __FILE__, __LINE__, "connect()", "host=127.0.0.1:8080");
 * @endcode
 */
int	logs_perror_raw(
	const t_log_level level,
	const char *const func,
	const char *const file,
	const int line,
	const char *const summary,
	const char *const body, ...
);

/**
 * @brief Log an already-built `t_log_report`.
 *
 * Forwards to internal `_logs(report, true, false)`.
 *
 * @param	report Log report tree root.
 *
 * @return Current implementation returns `error_none` (0) on write path.
 *
 * @version 2.0.0
 *
 * @code{.c}
 * t_log_report child = {.level = log_debug, .summary = "details"};
 * t_log_report root = {.level = log_info, .summary = "request done", .sub = &child};
 * logs_report(&root);
 * @endcode
 */
int	logs_report(
	t_log_report *const report
);

/**
 * @brief Log an already-built `t_log_report` and append `strerror(errno)`.
 *
 * Forwards to internal `_logs(report, true, true)`.
 *
 * @param	report Log report tree root.
 *
 * @return Current implementation returns `error_none` (0) on write path.
 *
 * @version 2.0.0
 *
 * @code{.c}
 * t_log_report rep = {.level = log_error, .summary = "open failed"};
 * logs_perror_report(&rep);
 * @endcode
 */
int	logs_perror_report(
	t_log_report *const report
);

/**
 * @brief Adapter used by `logs(...)` macro when first argument is a report.
 *
 * @param		report Report to forward.
 * @param	...	Ignored variadic placeholder kept for macro compatibility.
 *
 * @return Forwarded return from `logs_report(report)`.
 *
 * @version 2.0.0
 */
static inline int	_logs_report_adapter(
	t_log_report *const report,
	...
)
{
	return (logs_report(report));
}

/**
 * @brief Adapter used by `logs_perror(...)` macro for report pointers.
 *
 * @param	report	Report to forward.
 * @param	...		Ignored variadic placeholder kept for macro compatibility.
 *
 * @return Forwarded return from `logs_perror_report(report)`.
 *
 * @version 2.0.0
 */
static inline int	_logs_perror_report_adapter(
	t_log_report *const report,
	...
)
{
	return (logs_perror_report(report));
}

#endif	// LIB_INTERFACE_LOG_H
