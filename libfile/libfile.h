/*
	Do:
		#define JPR_LIBFILE_IMPLEMENTATION 
	to get access to the lib functions.
	This should be done only one time !
*/


#ifndef JPR_LIBFILE_H
# define JPR_LIBFILE_H

# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include "libstr.h"
# include "libmem.h"

# ifndef LIBFILE_BUFFER_SIZE
#  define LIBFILE_BUFFER_SIZE 1024
# endif

/**
 * @brief Reads data from @a fd until a line feed '\\n' is found, then returns the line.
 * @param size The size of the returned line in bytes.
 * @return The allocated line, with the line feed '\\n' at its end, or NULL on failure.
 */
void	*fileReadLine(int fd, size_t *size);

/**
 * @brief Reads data from @a fd until nothing's left.
 * @return The allocated file content, or NULL on failure
 */
char	*fileReadFd(int fd);

/**
 * @brief Opens the file located at @a path, then reads it entirely.
 * @return The allocated file content, or NULL on failure
 */
char	*fileRead(const char *path);

#endif // JPR_LIBFILE_H

#ifdef JPR_LIBFILE_IMPLEMENTATION
# ifndef JPR_LIBFILE_IMPLEMENTATION_DONE
#  define JPR_LIBFILE_IMPLEMENTATION_DONE

void	*fileReadLine(int fd, size_t *sizeptr)
{
	void			*tmp = NULL;
	void			*line = NULL;
	void			*buf = NULL;
	ssize_t			buf_size = 0;
	static void		*data = NULL;
	static size_t	data_size = 0;

	if (fd < 0 || LIBFILE_BUFFER_SIZE < 0)
		return (NULL);
	while (!memFindFirstByte(data, '\n', data_size))
	{
		buf = malloc(LIBFILE_BUFFER_SIZE);
		if (NULL == buf)
		{
			free(data);
			data = NULL;
			data_size = 0;
			if (sizeptr)
				*sizeptr = 0;
			return (NULL);
		}
		buf_size = read(fd, buf, LIBFILE_BUFFER_SIZE);
		if (-1 == buf_size)
		{
			free(buf);
			free(data);
			data = NULL;
			data_size = 0;
			if (sizeptr)
				*sizeptr = 0;
			return (NULL);
		}
		if (0 == buf_size)
		{
			line = memDup(data, data_size);
			if (sizeptr)
				*sizeptr = (NULL == line ? 0 : data_size);
			free(buf);
			free(data);
			data = NULL;
			data_size = 0;
			return (line);
		}
		tmp = malloc(data_size + buf_size);
		if (NULL == tmp)
		{
			free(buf);
			free(data);
			data = NULL;
			data_size = 0;
			if (sizeptr)
				*sizeptr = 0;
			return (NULL);
		}
		memCopy(tmp, data, data_size);
		memConcat(tmp, buf, data_size, buf_size);
		free(data);
		data = tmp;
		data_size += buf_size;
		free(buf);
	}
	size_t	offset = (char *)memFindFirstByte(data, '\n', data_size) - (char *)data + 1;
	line = memDup(data, offset);
	tmp = memDup(data + offset, data_size - offset);
	if (NULL == line || NULL == tmp)
	{
		free(line);
		free(data);
		data = NULL;
		data_size = 0;
		if (sizeptr)
			*sizeptr = 0;
		return (NULL);
	}
	free(data);
	data = tmp;
	data_size -= offset;
	if (sizeptr)
		*sizeptr = offset;
	return (line);
}

char	*fileReadFd(int fd)
{
	char	*data = NULL;
	char	*line;

	if (fd < 0)
		return (NULL);
	line = fileReadLine(fd, NULL);
	while (line)
	{
		if (NULL == strAppend(&data, line))
		{
			free(line);
			free(data);
			return (NULL);
		}
		free(line);
		line = fileReadLine(fd, NULL);
	}
	return (data);
}

char	*fileRead(const char *path)
{
	int		fd;
	char	*data = NULL;

	if (NULL == path)
		return (NULL);
	if (0 != access(path, F_OK | R_OK))
		return (NULL);
	fd = open(path, O_RDONLY);
	if (-1 == fd)
		return (NULL);
	data = fileReadFd(fd);
	close(fd);
	return (data);
}

# endif // JPR_LIBFILE_IMPLEMENTATION_DONE

#endif // JPR_LIBFILE_IMPLEMENTATION
