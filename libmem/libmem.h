/*
	Do:
		#define JPR_LIBMEM_IMPLEMENTATION 
	to get access to the lib functions.
	This should be done only one time !
*/


#ifndef JPR_LIBMEM_H
# define JPR_LIBMEM_H

# include <stdlib.h>

/**
 * @brief Sets the firsts @a n bytes from @a s to @a c.
 */
void	*memSet(void *s, int c, size_t n);

/**
 * @brief Returns a pointer to the first occurrence of @a c within the first @a n bytes of @a s.
 * @return A pointer to the occurrence, or NULL if @a c is not found.
 */
void	*memFindFirstByte(const void *s, int c, size_t n);

/**
 * @brief Returns a pointer to the last occurrence of @a c within the first @a n bytes of @a s.
 * @return A pointer to the occurrence, or NULL if @a c is not found.
 */
void	*memFindLastByte(const void *s, int c, size_t n);

/**
 * @brief Compares at most the first @a n bytes of @a s1 and @a s2.
 * @return A negative value if @a s1 is less than @a s2, a positive value if @a s1 is greater than @a s2, or 0 if they are equal.
 */
int	memCompare(const void *s1, const void *s2, size_t n);

/**
 * @brief Copies at most the first @a n bytes of @a src into @a dst.
 * @warning @a dst must be large enough to hold the copied bytes.
 * @return @a dst on success, or NULL on failure.
 */
void	*memCopy(void *dst, const void *src, size_t n);

/**
 * @brief Copies at most the first @a n bytes of @a src into @a dst. 
 * 
 * @a dst and @a src memory area may overlap.
 * @warning @a dst must be large enough to hold the copied bytes.
 * @return @a dst on success, or NULL on failure.
 */
void	*memMove(void *dst, const void *src, size_t n);

/**
 * @brief Concatenates the first @a src_size bytes of @a src at the end of @a dst.
 * @warning @a dst must be large enough to hold the result.
 * @return @a dst on success, or NULL on failure.
 */
void	*memConcat(void *dst, const void *src, size_t dst_size, size_t src_size);

/**
 * @brief Copies at most the first @a n bytes of @a data into an allocated buffer. 
 * @warning undefined behavior may occur in case @a n is greater than @a data size.
 * @return An allocated copy of @a data on success, or NULL on failure.
 */
void	*memDup(const void *data, size_t n);

#endif // JPR_LIBMEM_H

#ifdef JPR_LIBMEM_IMPLEMENTATION
# ifndef JPR_LIBMEM_IMPLEMENTATION_DONE
#  define JPR_LIBMEM_IMPLEMENTATION_DONE

void	*memSet(void *s, int c, size_t n)
{
	size_t	index = 0;

	if (NULL == s)
		return (NULL);
	while (index < n)
	{
		*((unsigned char *)s + index) = c;
		++index;
	}
	return (s);
}

void	*memFindFirstByte(const void *s, int c, size_t n)
{
	size_t		index = 0;

	if (NULL == s)
		return (NULL);
	while (index < n && *((const unsigned char *)s + index) != (unsigned char)c)
		++index;
	if (index >= n)
		return (NULL);
	return ((char *)s + index);
}

void	*memFindLastByte(const void *s, int c, size_t n)
{
	size_t	index = 0;
	void	*last = NULL;

	if (NULL == s)
		return (NULL);
	while (index < n)
	{
		if (*((const unsigned char *)s + index) == (unsigned char)c)
			last = (char *)s + index;
		++index;
	}
	return (last);
}

int	memCompare(const void *s1, const void *s2, size_t n)
{
	size_t				index = 0;
	const unsigned char	*u1 = (const unsigned char *)s1;
	const unsigned char	*u2 = (const unsigned char *)s2;

	if ((NULL == u1 && NULL == u2) || 0 == n)
		return (0);
	if (NULL == u1)
		return (-*u2);
	if (NULL == u2)
		return (*u1);
	while (*(u1 + index) && *(u2 + index) && *(u1 + index) == *(u2 + index) && index < n - 1)
		++index;
	return (*(u1 + index) - *(u2 + index));
}

void	*memCopy(void *dst, const void *src, size_t n)
{
	size_t	index = 0;

	if (NULL == dst)
		return (NULL);
	if (NULL == src || 0 == n)
		return (dst);
	while (index < n)
	{
		*((unsigned char *)dst + index) = *((unsigned char *)src + index);
		++index;
	}
	return (dst);
}

void	*memMove(void *dst, const void *src, size_t n)
{
	if (NULL == dst)
		return (NULL);
	if (NULL == src || 0 == n)
		return (dst);
	if (dst < src || dst > (void *)((char *)src + n))
		return (memCopy(dst, src, n));
	while (n > 0)
	{
		*((unsigned char *)dst + n - 1) = *((unsigned char *)src + n - 1);
		--n;
	}
	return (dst);	
}

void	*memConcat(void *dst, const void *src, size_t dst_size, size_t src_size)
{
	if (NULL == dst)
		return (NULL);
	if (NULL == src || 0 == src_size)
		return (dst);
	memCopy((char *)dst + dst_size, src, src_size);
	return (dst);
}

void	*memDup(const void *data, size_t n)
{
	void	*dup;

	if (NULL == data)
		return (NULL);
	dup = malloc(n);
	if (NULL == dup)
		return (NULL);
	memCopy(dup, data, n);
	return (dup);
}

# endif // JPR_LIBMEM_IMPLEMENTATION_DONE

#endif // JPR_LIBMEM_IMPLEMENTATION
