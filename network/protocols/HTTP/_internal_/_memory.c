/**
 * @file _memory.c
 * @brief Internal allocation and ownership helpers for HTTP objects.
 */

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdint.h>

	/* Internal */
#include "_HTTP.h"

	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */

/**
 * use `FNV-1a` for hashing
 * 
 * @note	the key should be normalized for it to work perfectly
*/
static uint32_t	_hash(
	const char *key
)
{
	uint32_t	hash = 2166136261u;

	while (*key)
	{
		hash ^= (unsigned char)*key++;
		hash *= 16777619u;
	}
	return (hash);
}

/* ----| Public     |----- */

struct _s_http_allocators	_g_net_prot_http_allocator = (struct _s_http_allocators){
	.alloc = mem_alloc,
	.realloc = mem_realloc,
	.free = mem_free
};

t_http	*_http_new(
	const float version,
	const t_http_methods method,
	const char *const path,
	const t_http_header_list *const headers,
	const t_http_body *const body
)
{
	t_http	*result = NULL;
	size_t	length = 0;

	result = _g_net_prot_http_allocator.alloc(sizeof(t_http));
	if (unlikely(!result))
	{
		g_net_prot_http_settings.errno = error_alloc_fail;
		return (NULL);
	}

	result->version = version;
	result->method = method;
	result->path = _g_net_prot_http_allocator.alloc(sizeof(char) * (strlen(path) + 1));
	if (unlikely(!result->path))
	{
		g_net_prot_http_settings.errno = error_alloc_fail;
		_g_net_prot_http_allocator.free(result);
		return (NULL);
	}
	strcpy(result->path, path);
	if (headers)
		result->headers = *headers;
	if (body)
	{
		result->body = *body;
		if (body->capacity)
		{
			length = strlen(body->content);
			result->content_length = length;
		}
		else
			result->content_length = 0;
	}
	else
		result->content_length = 0;

	return (result);
}

t_http_header	*_http_new_header(
	const char *const key,
	const char *const value
)
{
	size_t			alloc_size = sizeof(t_http_header) + (sizeof(char) * (strlen(key) + 1));
	t_http_header	*result = NULL;

	result = _g_net_prot_http_allocator.alloc(alloc_size);
	if (unlikely(!result))
	{
		g_net_prot_http_settings.errno = HTTP_ERROR_SYSCALL;
		return (NULL);
	}

	result->value = _g_net_prot_http_allocator.alloc(sizeof(char) * strlen(value));
	if (unlikely(!result->value))
	{
		_g_net_prot_http_allocator.free(result);
		g_net_prot_http_settings.errno = HTTP_ERROR_SYSCALL;
		return (NULL);
	}
	result->key = (char *)(result + 1);
	strcpy(result->key, key);
	_lower(result->key);
	strcpy(result->value, value);

	result->next = NULL;
	result->order_prev = NULL;
	result->order_next = NULL;

	return (result);
}

int	_http_setup_header_list(
	t_http_header_list *const list,
	const int capacity
)
{
	t_http_header	**map = NULL;

	map = _g_net_prot_http_allocator.alloc(sizeof(t_http_header *) * capacity);
	if (unlikely(!map))
	{
		g_net_prot_http_settings.errno = HTTP_ERROR_SYSCALL;
		return (error_alloc_fail);
	}
	list->capacity = capacity;
	list->data = map;
	list->head = NULL;
	list->tail = NULL;

	return (error_none);
}

int	_http_add_header(
	t_http_header_list *const list,
	t_http_header *const header
)
{
	const int		_index = _hash(header->key) % list->capacity;
	t_http_header	*_this = list->data[_index];

	header->next = NULL;
	header->order_next = NULL;
	header->order_prev = list->tail;

	if (list->tail)
		list->tail->order_next = header;
	else
		list->head = header;

	list->tail = header;

	if (!_this)
	{
		list->data[_index] = header;
	}
	else
	{
		while (_this->next)
			_this = _this->next;
		_this->next = header;
	}

	list->size++;
	return (error_none);
}

void	_http_remove_header(
	t_http_header_list *const list,
	const char *const key,
	const int do_free
)
{
	if (!list->size)
		return ;

	const int		_index = _hash(key) % list->capacity;
	t_http_header	*_this = list->data[_index];
	t_http_header	*_prev = NULL;

	while (_this)
	{
		if (!strcmp(_this->key, key))
		{
			// unlink hash chain
			if (_prev)
				_prev->next = _this->next;
			else
				list->data[_index] = _this->next;

			// unlink order list
			if (_this->order_prev)
				_this->order_prev->order_next = _this->order_next;
			else
				list->head = _this->order_next;

			if (_this->order_next)
				_this->order_next->order_prev = _this->order_prev;
			else
				list->tail = _this->order_prev;

			list->size--;

			if (do_free)
				_http_free_header(_this, 0);

			return ;
		}

		_prev = _this;
		_this = _this->next;
	}
}

t_http_header	*_http_header_find(
	t_http_header_list *const list,
	const char *const key
)
{
	const uint32_t	_index = _hash(key) % list->capacity;

	for (t_http_header	*_this = list->data[_index];
		_this != NULL;
		_this = _this->next
	)
	{
		if (unlikely(!strcmp(_this->key, key)))
			return (_this);
	}

	return (NULL);
}

/**
 * if `all` == 1 also free the list of header
*/
void	_http_free(
	t_http *const target,
	const int all
)
{
	if (all)
		_http_free_list(&target->headers, 1);

	_g_net_prot_http_allocator.free(target->path);
	_g_net_prot_http_allocator.free(target);
}

void	_http_free_header(
	t_http_header *const header,
	const int recursive
)
{
	if (recursive && header->next)
		_http_free_header(header->next, true);

	if (header->value)
		_g_net_prot_http_allocator.free(header->value);
	
	_g_net_prot_http_allocator.free(header);
}

/**
 * if `recursive` == 1, free all element of the list
*/
void	_http_free_list(
	t_http_header_list *const list,
	const int recursive
)
{
	for (int	 i = 0;
		recursive && i < list->capacity;
		i++
	)
	{
		t_http_header	*_next = NULL;

		if (!list->data[i])
			continue ;

		for (t_http_header	*_this = list->data[i];
			_this != NULL;
			_this = _next
		)
		{
			_next = _this->next;
			_http_free_header(_this, recursive);
		}
	}

	_g_net_prot_http_allocator.free(list->data);
}