// Header

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
#include <stddef.h>

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */
#if MEM_MANAGER_TYPE == 0

typedef struct s_mem_node		t_mem_node;

#elif MEM_MANAGER_TYPE == 1

typedef struct s_rb_node		rb_node;
typedef struct s_rb_tree		rb_tree;

#endif	// MEM_MANAGER_TYPE

typedef struct s_mem_manager	t_mem_manager;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

enum e_mem_manager_access
{
	mem_manager_access_alloc,
	mem_manager_access_realloc,
	mem_manager_access_dup,
	mem_manager_access_free,
	mem_manager_access_free_all,
};

enum e_mem_manager_error
{
	mem_manager_error_none = error_none,
	mem_manager_error_alloc_failed = error_alloc_fail,
	mem_manager_error_register_failed,
	mem_manager_error_unregister_failed,
	mem_manager_error_node_not_found,
};

typedef enum { RED, BLACK } rb_color;

/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */

#if MEM_MANAGER_TYPE == 0

struct s_mem_node
{
	void		*content;
	size_t		size;
	t_mem_node	*next;
};

#elif MEM_MANAGER_TYPE == 1

struct s_rb_node
{
	void		*key;		// address of the return memory area
	size_t		size;		// size of the allocated memory area
	rb_color	color;		// color of the node
	rb_node		*left;		// left child of the node
	rb_node		*right;		// right child of the node
	rb_node		*parent;	
};

struct s_rb_tree
{
	rb_node	*root;	// root of the rb tree
	rb_node	*nil;	// sentinel of the rb tree
};


#else

#error "MEM_MANAGER_TYPE is invalide"

#endif	// MEM_MANAGER_TYPE

struct s_mem_manager
{
	#if MEM_MANAGER_TYPE == 0

	t_mem_node	*bucket[MEM_BUCKET_SIZE];

	#elif MEM_MANAGER_TYPE == 1

	rb_tree	*T;	// the red black tree to store allocated memory

	#endif	// MEM_MANAGER_TYPE
};
