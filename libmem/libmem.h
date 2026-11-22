/*
	Do:
		#define JPR_LIBMEM_IMPLEMENTATION 
	to get access to the lib functions.
	This should be done only one time !
*/


#ifndef JPR_LIBMEM_H
# define JPR_LIBMEM_H

# include <stdlib.h>
# include <ctype.h>

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

# ifdef	JPR_LIBMEM_IMPLEMENTATION

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
	return ((void *)s + index);
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
			last = (void *)s + index;
		++index;
	}
	return (last);
}

int	memCompare(const void *s1, const void *s2, size_t n)
{
	size_t	index = 0;

	if ((NULL == s1 && NULL == s2) || 0 == n)
		return (0);
	if (NULL == s1)
		return (-*(unsigned char *)s2);
	if (NULL == s2)
		return (*(unsigned char *)s1);
	while (*((unsigned char *)s1 + index) && *((unsigned char *)s2 + index) && *((unsigned char *)s1 + index) == *((unsigned char *)s2 + index) && index < n - 1)
		++index;
	return (*((unsigned char *)s1 + index) - *((unsigned char *)s2 + index));
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
	if (dst < src || dst > src + n)
		return (memCopy(dst, src, n));
	while (n > 0)
	{
		*((unsigned char *)dst + n - 1) = *((unsigned char *)src + n - 1);
		--n;
	}
	return (dst);	
}

# endif // JPR_LIBMEM_IMPLEMENTATION

#endif // JPR_LIBMEM_H
