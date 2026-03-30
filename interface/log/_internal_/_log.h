/**
 * @file _log.h
 * @brief Internal logging pipeline API for `interface/log`.
 *
 * Declarations in this file are private to the module implementation.
 */

#ifndef	LIB_INTERFACE_LOG_INTERNAL_H
# define LIB_INTERFACE_LOG_INTERNAL_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "lib_config.h"
# include "lib_standards.h"

/* -----| Internals |----- */
# include "_types.h"
#include "../../log.h"

/* -----| Modules   |----- */
#include "lib.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

/**
 * @brief Initialize the internal singleton content from public init payload.
 *
 * Opens file descriptors per log level and marks config as initialized.
 *
 * @param config Internal singleton storage.
 * @param data   Public initialization payload.
 *
 * @return `error_none` on success.
 * @return `error_invalid_arg` when `config` or `data` is NULL.
 *
 * @code{.c}
 * t_log_internal cfg;
 * t_log_init init = {.display_limit = log_other};
 * int rc = _log_init(&cfg, &init);
 * @endcode
 */
int	_log_init(
	t_log_internal *const config,
	const t_log_init *const data
);

/**
 * @brief Access or update the module static configuration singleton.
 *
 * When `init != NULL`, applies initialization through `_log_init(...)`.
 * Always returns the static singleton address.
 *
 * @param init Optional public init payload.
 *
 * @return Pointer to the module-owned singleton (never NULL currently).
 *
 * @code{.c}
 * t_log_internal *cfg = _log_config(NULL);
 * @endcode
 */
t_log_internal	*_log_config(
	const t_log_init *const init
);

/**
 * @brief Print one report tree to an fd using incremental writes.
 *
 * Handles indentation and recursion through `report->sub`.
 *
 * @param report Report tree root.
 * @param fd     Target file descriptor.
 * @param depth  Initial indentation depth.
 *
 * @return `0` in current implementation.
 *
 * @code{.c}
 * _logs_print(&report, STDERR_FILENO, 0);
 * @endcode
 */
int	_logs_print(
	const t_log_report *report,
	const int fd,
	const int depth
);

/**
 * @brief Print one report tree using a bounded atomic-style buffer path.
 *
 * The full tree is appended into a temporary buffer then written in one loop.
 *
 * @param report Report tree root.
 * @param fd     Target file descriptor.
 *
 * @return `0` in current implementation.
 *
 * @code{.c}
 * _logs_print_atomic(&report, fd);
 * @endcode
 */
int	_logs_print_atomic(
	const t_log_report *report,
	const int fd
);

/**
 * @brief Internal dispatcher for file and terminal logging targets.
 *
 * Chooses output fds from singleton config, optionally enriches summary with
 * `strerror(errno)`, then routes to atomic or non-atomic printing.
 *
 * @param report       Mutable report payload.
 * @param atomic_safe  Use `_logs_print_atomic()` when true.
 * @param perr         Append strerror text to summary when true.
 *
 * @return Last printing return code (currently `0` in normal path).
 *
 * @code{.c}
 * _logs(&report, true, false);
 * @endcode
 */
int	_logs(
	t_log_report *const report,
	const bool atomic_safe,
	const bool perr
);

#endif	// LIB_INTERFACE_LOG_INTERNAL_H
