/*
	Do:
		#define JPR_LIBHASHTABLE_IMPLEMENTATION 
	to get access to the lib functions.
	This should be done only one time !
*/


#ifndef JPR_LIBHASHTABLE_H
# define JPR_LIBHASHTABLE_H

# include <stdlib.h>
# include "libllist.h"

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

/**
 * @brief Sets the destroy function that is called on the entry data when it needs to be destroyed.
 */
void	hashtableEntryFuncDestroySet(t_hashtableEntryFuncDestroy fn);

/**
 * @brief Sets the dup function that is called on the entry data when it needs to be duplicated.
 */
void	hashtableEntryFuncDupSet(t_hashtableEntryFuncDup fn);

#endif // JPR_LIBHASHTABLE_H

#ifdef JPR_LIBHASHTABLE_IMPLEMENTATION
# ifndef JPR_LIBHASHTABLE_IMPLEMENTATION_DONE
#  define JPR_LIBHASHTABLE_IMPLEMENTATION_DONE

# include "libstr.h"
# include "libmem.h"

t_hashtableEntryFuncDestroy	__private__hashtableEntryDestroyFunc = NULL;
t_hashtableEntryFuncDup		__private__hashtableEntryDupFunc = NULL;

void	hashtableEntryFuncDestroySet(t_hashtableEntryFuncDestroy fn)
{
	__private__hashtableEntryDestroyFunc = fn;
	return ;
}

void	hashtableEntryFuncDupSet(t_hashtableEntryFuncDup fn)
{
	__private__hashtableEntryDupFunc = fn;
	return ;
}

static size_t	__private__hashTableHash(const char *key)
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

static t_hashtableEntry	*__private__hashtableEntryNew(const char *key, void *data)
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

static t_hashtableEntry	*__private__hashtableEntryDup(const t_hashtableEntry *src)
{
	t_hashtableEntry	*dup;
	void				*data;

	data = __private__hashtableEntryDupFunc(src->data);
	if (NULL == data)
		return (NULL);
	dup = __private__hashtableEntryNew(src->key, data);
	if (NULL == dup)
		return (NULL);
	return (dup);
}


static int 	__private__hashtableEntryCompareKeys(const char *key1, const char *key2)
{
	return (strCompare(key1, key2));
}

static void	__private__hashtableEntryDestroy(t_hashtableEntry *entry)
{
	if (NULL == entry)
		return ;
	free(entry->key);
	if (__private__hashtableEntryDestroyFunc)
		__private__hashtableEntryDestroyFunc(entry->data);
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
	hashv = __private__hashTableHash(key);
	entry = __private__hashtableEntryNew(key, data);
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
		memSet(table->data + table->capacity, 0, (hashv + 1 - table->capacity) * sizeof(t_llist *));
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

	t_llist	*node = llistGetFor(*(table->data + hashv), (t_llistCompareFn)&__private__hashtableEntryCompareKeys, key);
	if (NULL == node)
	{
		if (NULL == llistPushBack(table->data + hashv, llistNew(entry)))
		{
			free(entry);
			return (NULL);
		}
		return (table);
	}
	if (__private__hashtableEntryDupFunc)
	{
		((t_hashtableEntry *)node->data)->data = __private__hashtableEntryDupFunc(data);
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
	hashv = __private__hashTableHash(key);
	if (hashv >= table->capacity)
		return (NULL);
	node = llistGetFor(*(table->data + hashv), (t_llistCompareFn)&__private__hashtableEntryCompareKeys, key);
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
			*(cpy->data + index) = llistDup(*(src->data + index), (t_llistDupFn)&__private__hashtableEntryDup, (t_llistDestroyFn)&__private__hashtableEntryDestroy);
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
			llistDestroy(table->data + i, (t_llistDestroyFn)&__private__hashtableEntryDestroy);
		}
		free(table->data);
		table->data = NULL;
	}
	free(table);
	return ;
}

# endif // JPR_LIBHASHTABLE_IMPLEMENTATION_DONE

#endif // JPR_LIBHASHTABLE_IMPLEMENTATION
