#include "get_next_line_bonus.h"

static char	*extract_line(const char *stash)
{
	size_t	len;
	char	*nl;

	if (!stash || stash[0] == '\0')
		return (NULL);
	nl = ft_strchr(stash, '\n');
	if (nl)
		len = (size_t)(nl - stash) + 1;
	else
		len = ft_strlen(stash);
	return (ft_substr(stash, 0, len));
}

static char	*trim_stash(char *stash)
{
	char	*nl;
	char	*remainder;

	nl = ft_strchr(stash, '\n');
	if (!nl)
	{
		free(stash);
		return (NULL);
	}
	remainder = ft_substr(stash, (unsigned int)(nl - stash) + 1,
			ft_strlen(nl + 1));
	free(stash);
	return (remainder);
}

static char	*read_to_stash(int fd, char *stash)
{
	char	buf[BUFFER_SIZE + 1];
	ssize_t	bytes;

	while (!ft_strchr(stash, '\n'))
	{
		bytes = read(fd, buf, BUFFER_SIZE);
		if (bytes < 0)
		{
			free(stash);
			return (NULL);
		}
		if (bytes == 0)
			break ;
		buf[bytes] = '\0';
		if (!stash)
		{
			stash = ft_substr(buf, 0, (size_t)bytes);
			if (!stash)
				return (NULL);
		}
		else
		{
			stash = ft_strjoin(stash, buf);
			if (!stash)
				return (NULL);
		}
	}
	return (stash);
}

/*
	get_next_line (bonus):

	Uses a SINGLE static variable — an array of stash pointers indexed by fd.
	This satisfies both bonus requirements:
		1. Only one static variable.
		2. Multiple file descriptors tracked independently and concurrently.

	Each index in stash[] holds the unconsumed leftover for that fd between calls.
	The array is zero-initialised by C's static storage rules (all NULLs at start).
*/
char	*get_next_line(int fd)
{
	static char	*stash[1024];
	char		*line;

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);
	stash[fd] = read_to_stash(fd, stash[fd]);
	if (!stash[fd])
		return (NULL);
	line = extract_line(stash[fd]);
	stash[fd] = trim_stash(stash[fd]);
	return (line);
}
