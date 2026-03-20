/**
 * 
*/

/* ----| Headers    |----- */
	/* Standard */
#include <string.h>
#include <stdint.h>
#include <ctype.h>

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

static void	_lower(
	char *str
)
{
	while (*str)
	{
		*str = tolower(*str);
		str++;
	}
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
	t_http_header_list headers,
	t_http_body body
)
{
	t_http	*result = NULL;
	size_t	length = 0;

	result = _g_net_prot_http_allocator.alloc(sizeof(t_http));
	if (unlikely(!result))
	{
		g_net_prot_http_settings.errno = HTTP_ERROR_SYSCALL;
		return (NULL);
	}

	result->version = version;
	result->method = method;
	result->headers = headers;
	result->body = body;
	if (body.capacity)
	{
		length = strlen(body.content);
		result->content_length = length;
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

	return (error_none);
}

int	_http_add_header(
	t_http_header_list *const list,
	t_http_header *const header
)
{
	const int		_index = _hash(header->key) % list->capacity;
	t_http_header	*_target = list->data[_index];

	for (t_http_header	*_this = list->data[_index];
		_this != NULL;
		_this = _this->next
	)
	{
		if (unlikely(!strcmp(_this->key, header->key)))
		{
			t_http_header	*_next = _this->next;

			_target->next = header;
			header->next = _next;
			list->size++;

			return (error_none);
		}

		_target = _this;
	}

	if (_target)
		_target->next = header;
	else
		list->data[_index] = header;

	return (error_none);
}

void	_http_remove_header(
	t_http_header_list *const list,
	const char *const key,
	const int free
)
{
	if (!list->size)
		return ;

	const int		_index = _hash(key) % list->capacity;
	t_http_header	*_target = list->data[_index];

	for (t_http_header	*_this = list->data[_index];
		_this != NULL;
		_this = _this->next
	)
	{
		if (unlikely(!strcmp(_this->key, key)))
		{
			t_http_header	*_next = _this->next;

			if (free)
				_g_net_prot_http_allocator.free(_this);

			_target->next = _next;

			return ;
		}

		_target = _this;
	}
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