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
# include "libstr.h"
# include <stdio.h>

# ifndef LIBFILE_BUFFER_SIZE
#  define LIBFILE_BUFFER_SIZE 1024
# endif

char	*fileReadLine(int fd);

#endif // JPR_LIBFILE_H

#ifdef JPR_LIBFILE_IMPLEMENTATION
# ifndef JPR_LIBFILE_IMPLEMENTATION_DONE
#  define JPR_LIBFILE_IMPLEMENTATION_DONE

char	*fileReadLine(int fd)
{
	int			nread;
	char		*tmp = NULL;
	char		*line = NULL;
	static char	*data = NULL;

	if (fd < 0 || LIBFILE_BUFFER_SIZE < 0)
		return (NULL);
	while (!strFindFirstOfChar(data, '\n'))
	{
		tmp = malloc(sizeof(char) * LIBFILE_BUFFER_SIZE + 1);
		if (NULL == tmp)
		{
			free(data);
			return (NULL);
		}
		nread = read(fd, tmp, LIBFILE_BUFFER_SIZE);
		if (-1 == nread)
		{
			free(tmp);
			free(data);
			return (NULL);
		}
		if (0 == nread)
		{
			line = strDup(data);
			free(tmp);
			free(data);
			data = NULL;
			return (line);
		}
		tmp[nread] = '\0';
		if (NULL == strAppend(&data, tmp))
		{
			free(tmp);
			free(data);
			return (NULL);
		}
		free(tmp);
	}
	line = strSub(data, 0, strFindFirstOfChar(data, '\n') - data);
	tmp = strSub(data, strFindFirstOfChar(data, '\n') - data + 1, (size_t)-1);
	if (NULL == line || NULL == tmp)
	{
		free(line);
		free(data);
		return (NULL);
	}
	free(data);
	data = tmp;
	return (line);
}

# endif // JPR_LIBFILE_IMPLEMENTATION_DONE

#endif // JPR_LIBFILE_IMPLEMENTATION
