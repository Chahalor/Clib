// Header

/* ----| Headers    |----- */
	/* Standard */
		//...
	/* Internal */
#include "_memory.h"
	/* External */
		//...

/* ----| Prototypes |----- */
	//...

/* ----| Internals  |----- */
	//...

/* ----| Public     |----- */


// --- utilitaires ---
#if MEM_MANAGER_TYPE == 1

rb_node	*rb_create_node(
	rb_tree *T,
	size_t size
)
{
	rb_node	*node = calloc(1, sizeof(rb_node) + size);

	node->key = (node + 1);
	node->size = size;
	node->color = RED;
	node->left = node->right = node->parent = T->nil;
	return (node);
}

rb_tree	*rb_init(void)
{
	rb_tree	*T = malloc(sizeof(rb_tree));
	rb_node	*nil = malloc(sizeof(rb_node));

	nil->color = BLACK;
	nil->left = nil->right = nil->parent = NULL;
	T->nil = nil;
	T->root = nil;
	return (T);
}

// --- rotations ---
static void	rb_left_rotate(
	rb_tree *T,
	rb_node *x
)
{
	rb_node	*y = x->right;

	x->right = y->left;
	if (y->left != T->nil)
		y->left->parent = x;
	y->parent = x->parent;
	if (x->parent == T->nil)
		T->root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;
	y->left = x;
	x->parent = y;
}

static void	rb_right_rotate(
	rb_tree *T,
	rb_node *y
)
{
	rb_node	*x = y->left;

	y->left = x->right;
	if (x->right != T->nil)
		x->right->parent = y;
	x->parent = y->parent;
	if (y->parent == T->nil)
		T->root = x;
	else if (y == y->parent->right)
		y->parent->right = x;
	else
		y->parent->left = x;
	x->right = y;
	y->parent = x;
}

// --- insertion fixup ---
static void	rb_insert_fixup(
	rb_tree *T,
	rb_node *z
)
{
	while (z->parent->color == RED)
	{
		if (z->parent == z->parent->parent->left)
		{
			rb_node	*y = z->parent->parent->right;
			if (y->color == RED)
			{
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}
			else
			{
				if (z == z->parent->right)
				{
					z = z->parent;
					rb_left_rotate(T, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rb_right_rotate(T, z->parent->parent);
			}
		}
		else
		{
			rb_node	*y = z->parent->parent->left;
			if (y->color == RED)
			{
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			}
			else
			{
				if (z == z->parent->left)
				{
					z = z->parent;
					rb_right_rotate(T, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rb_left_rotate(T, z->parent->parent);
			}
		}
	}
	T->root->color = BLACK;
}

// --- insertion ---
void	rb_insert(
	rb_tree *T,
	rb_node *z
)
{
	rb_node	*y = T->nil;
	rb_node	*x = T->root;

	while (x != T->nil)
	{
		y = x;
		if (z->key < x->key)
			x = x->left;
		else
			x = x->right;
	}
	z->parent = y;
	if (y == T->nil)
		T->root = z;
	else if (z->key < y->key)
		y->left = z;
	else
		y->right = z;

	rb_insert_fixup(T, z);
}

// --- recherche ---
rb_node	*rb_search(
	rb_tree *T,
	void *key
)
{
	rb_node	*x = T->root;
	while (x != T->nil && key != x->key)
	{
		if (key < x->key)
			x = x->left;
		else
			x = x->right;
	}
	return (x == T->nil) ? NULL : x;
}

// --- minimum ---
static rb_node	*rb_minimum(
	rb_tree *T,
	rb_node *x
)
{
	while (x->left != T->nil)
		x = x->left;
	return x;
}

// --- transplant (utilitaire pour suppression) ---
static void	rb_transplant(
	rb_tree *T,
	rb_node *u,
	rb_node *v
)
{
	if (u->parent == T->nil)
		T->root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}

// --- suppression fixup ---
static void	rb_delete_fixup(
	rb_tree *T,
	rb_node *x
)
{
	while (x != T->root && x->color == BLACK)
	{
		if (x == x->parent->left)
		{
			rb_node	*w = x->parent->right;

			if (w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;
				rb_left_rotate(T, x->parent);
				w = x->parent->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK)
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{
				if (w->right->color == BLACK)
				{
					w->left->color = BLACK;
					w->color = RED;
					rb_right_rotate(T, w);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rb_left_rotate(T, x->parent);
				x = T->root;
			}
		}
		else
		{
			rb_node	*w = x->parent->left;

			if (w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;
				rb_right_rotate(T, x->parent);
				w = x->parent->left;
			}
			if (w->right->color == BLACK && w->left->color == BLACK)
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{
				if (w->left->color == BLACK)
				{
					w->right->color = BLACK;
					w->color = RED;
					rb_left_rotate(T, w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rb_right_rotate(T, x->parent);
				x = T->root;
			}
		}
	}
	x->color = BLACK;
}

// --- suppression ---
void	rb_delete(
	rb_tree *T,
	rb_node *z,
	const int _free
)
{
	rb_node		*y = z;
	rb_color	y_original_color = y->color;
	rb_node		*x;

	if (z->left == T->nil)
	{
		x = z->right;
		rb_transplant(T, z, z->right);
	}
	else if (z->right == T->nil)
	{
		x = z->left;
		rb_transplant(T, z, z->left);
	}
	else
	{
		y = rb_minimum(T, z->right);
		y_original_color = y->color;
		x = y->right;
		if (y->parent == z) x->parent = y;
		else {
			rb_transplant(T, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		rb_transplant(T, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}

	if (y_original_color == BLACK)
		rb_delete_fixup(T, x);
	if (_free)
		free(z);
}

// // --- inorder print (debug) ---
// void	rb_inorder(
// 	rb_tree *T,
// 	rb_node *x
// )
// {
// 	if (x != T->nil)
// 	{
// 		rb_inorder(T, x->left);
// 		printf("key=%p size=%zu color=%s\n", x->key, x->size, x->color == RED ? "RED":"BLACK");
// 		rb_inorder(T, x->right);
// 	}
// }

#endif	// MEM_MANAGER_TYPE