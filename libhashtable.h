/*
	Do:
		#define LIBHASHTABLE_IMPLEMENTATION 
	to get access to the lib functions.
	This should be done only one time !
*/


#ifndef LIBHASHTABLE_H
# define LIBHASHTABLE_H

# include <stdlib.h>

# ifdef LIBHASHTABLE_IMPLEMENTATION
#  ifndef LIBLLIST_IMPLEMENTATION
#   define LIBLLIST_IMPLEMENTATION
#  endif
# endif
# include "libs/libllist/libllist.h"

typedef void	(*t_hashtableEntryFuncDestroy)(void *data);
typedef void	*(*t_hashtableEntryFuncDup)(const void *data);

typedef struct s_hashTableEntry
{
	char	*key;
	void	*data;
}	t_hashtableEntry;

typedef struct s_hashTable
{
	/**
	 * @brief The size of data in bytes
	 */
	size_t	bsize;
	/**
	 * @brief The number of entries that can be stored
	 */
	size_t	capacity;
	/**
	 * @brief Pointer to the data array
	 * @warning It is not NULL terminated !
	 */
	t_llist	**data;
}	t_hashtable;

/**
 * @brief Creates a new hash table.
 * @param initial_capacity The initial number of entries the table can store.
 * @return A pointer to the newly created hash table.
 */
t_hashtable	*hashtableNew(size_t initial_capacity);

/**
 * @brief Stores data inside the table if key is inexistent, updates it otherwise.
 * @return The updated hash table.
 */
t_hashtable	*hashtableSet(t_hashtable *table, const char *key, void *data);

/**
 * @brief Searches the corresponding node inside the table.
 * @return A pointer to the key node.
 */
t_hashtableEntry	*hashtableGet(const t_hashtable *table, const char *key);

/**
 * @brief Creates an allocated copy of src.
 * @return A pointer to the copy.
 */
t_hashtable	*hashtableDup(const t_hashtable *src);

/**
 * @brief Destroys allocated data and the hash table pointer.
 */
void	hashtableDestroy(t_hashtable *table);

void	hashtableEntryFuncDestroySet(t_hashtableEntryFuncDestroy fn);
void	hashtableEntryFuncDupSet(t_hashtableEntryFuncDup fn);

# ifdef LIBHASHTABLE_IMPLEMENTATION

# ifndef LIBSTR_IMPLEMENTATION
#  define LIBSTR_IMPLEMENTATION
# endif
# include "libs/libstr/libstr.h"

t_hashtableEntryFuncDestroy	hashtableEntryDestroyFunc = NULL;
t_hashtableEntryFuncDup		hashtableEntryDupFunc = NULL;

void	hashtableEntryFuncDestroySet(t_hashtableEntryFuncDestroy fn)
{
	hashtableEntryDestroyFunc = fn;
	return ;
}

void	hashtableEntryFuncDupSet(t_hashtableEntryFuncDup fn)
{
	hashtableEntryDupFunc = fn;
	return ;
}

static size_t	private_hashTableHash(const char *key)
{
	size_t	hashv = 0;

	if (NULL == key)
		return (0);
	while (*key)
	{
		hashv += *key;
		++key;
	}
	return (hashv);
}

static t_hashtableEntry	*private_hashtableEntryNew(const char *key, void *data)
{
	t_hashtableEntry	*new;

	new = malloc(sizeof(t_hashtableEntry));
	if (NULL == new)
		return (NULL);
	new->key = strDup(key);
	if (NULL == new->key)
	{
		free(new);
		return (NULL);
	}
	new->data = data;
	return (new);
}

static t_hashtableEntry	*private_hashtableEntryDup(const t_hashtableEntry *src)
{
	t_hashtableEntry	*dup;
	void				*data;

	data = hashtableEntryDupFunc(src->data);
	if (NULL == data)
		return (NULL);
	dup = private_hashtableEntryNew(src->key, data);
	if (NULL == dup)
		return (NULL);
	return (dup);
}


static int 	private_hashtableEntryCompareKeys(const char *key1, const char *key2)
{
	return (strCompare(key1, key2));
}

static void	private_hashtableEntryDestroy(t_hashtableEntry *entry)
{
	if (NULL == entry)
		return ;
	free(entry->key);
	if (hashtableEntryDestroyFunc)
		hashtableEntryDestroyFunc(entry->data);
	free(entry);
	return ;
}

t_hashtable	*hashtableNew(size_t initial_capacity)
{
	t_hashtable	*new;

	new = malloc(sizeof(t_hashtable));
	if (NULL == new)
		return (NULL);
	new->bsize = initial_capacity * sizeof(t_hashtableEntry *);
	new->capacity = initial_capacity;
	new->data = calloc(1, new->bsize);
	if (NULL == new->data)
	{
		free(new);
		return (NULL);
	}
	return (new);
}

t_hashtable	*hashtableSet(t_hashtable *table, const char *key, void *data)
{
	size_t		hashv;
	t_hashtableEntry	*entry;

	if (NULL == table || NULL == key)
		return (NULL);
	hashv = private_hashTableHash(key);
	entry = private_hashtableEntryNew(key, data);
	if (NULL == entry)
		return (NULL);
	if (hashv >= table->capacity)
	{
		t_llist	**old_data = table->data;

		table->data = realloc(table->data, (hashv + 1) * sizeof(t_llist *));
		if (NULL == table->data)
		{
			table->data = old_data;
			free(entry);
			return (NULL);
		}
		strMemSet(table->data + table->capacity, 0, (hashv + 1 - table->capacity) * sizeof(t_llist *));
		table->bsize = (hashv + 1) * sizeof(t_llist *);
		table->capacity = hashv + 1;
		if (NULL == llistPushBack(table->data + hashv, llistNew(entry)))
		{
			free(entry);
			return (NULL);
		}
		return (table);
	}

	if (NULL == *(table->data + hashv))
	{
		*(table->data + hashv) = llistNew(entry);
		if (NULL == *(table->data + hashv))
		{
			free(entry);
			return (NULL);
		}
		return (table);
	}

	t_llist	*node = llistGetFor(*(table->data + hashv), (t_llistCompareFn)&private_hashtableEntryCompareKeys, key);
	if (NULL == node)
	{
		if (NULL == llistPushBack(table->data + hashv, llistNew(entry)))
		{
			free(entry);
			return (NULL);
		}
		return (table);
	}
	if (hashtableEntryDupFunc)
	{
		((t_hashtableEntry *)node->data)->data = hashtableEntryDupFunc(data);
		if (NULL == ((t_hashtableEntry *)node->data)->data)
			return (NULL);
	}
	return (table);
}

t_hashtableEntry	*hashtableGet(const t_hashtable *table, const char *key)
{
	size_t	hashv;
	t_llist	*node;

	if (NULL == table || NULL == key)
		return (NULL);
	hashv = private_hashTableHash(key);
	if (hashv >= table->capacity)
		return (NULL);
	node = llistGetFor(*(table->data + hashv), (t_llistCompareFn)&private_hashtableEntryCompareKeys, key);
	if (NULL == node)
		return (NULL);
	return (node->data);
}

t_hashtable	*hashtableDup(const t_hashtable *src)
{
	size_t		index = 0;
	t_hashtable	*cpy;

	if (NULL == src)
		return (NULL);
	cpy = hashtableNew(src->capacity);
	if (NULL == cpy)
		return (NULL);
	while (index < src->capacity)
	{
		if (NULL != *(src->data + index))
		{
			*(cpy->data + index) = llistDup(*(src->data + index), (t_llistDupFn)&private_hashtableEntryDup, (t_llistDestroyFn)&private_hashtableEntryDestroy);
			if (NULL == *(cpy->data + index))
			{
				hashtableDestroy(cpy);
				return (NULL);
			}
		}
		++index;
	}
	return (cpy);
}

void	hashtableDestroy(t_hashtable *table)
{
	if (NULL == table)
		return ;
	if (NULL != table->data)
	{
		for (size_t i = 0; i < table->capacity; ++i)
		{
			if (NULL == *(table->data + i))
				continue;
			llistDestroy(table->data + i, (t_llistDestroyFn)&private_hashtableEntryDestroy);
		}
		free(table->data);
		table->data = NULL;
	}
	free(table);
	return ;
}

# endif // LIBHASHTABLE_IMPLEMENTATION

#endif // LIBHASHTABLE_H
