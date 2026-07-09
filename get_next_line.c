/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbali <dbali@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 12:01:01 by dbali             #+#    #+#             */
/*   Updated: 2026/07/09 13:25:17 by dbali            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*extract_line(const char *stash)
{
	size_t	len;
	char	*nl;

	if (!stash || stash[0] == '\0')
		return (NULL);
	nl = ft_strchr(stash, '\n');
	if (nl)
		len = (nl - stash) + 1;
	else
		len = ft_strlen(stash);
	return (ft_substr(stash, 0, len));
}

static char	*append_to_stash(char *stash, char *buf, ssize_t bytes)
{
	if (!stash)
		return (ft_substr(buf, 0, bytes));
	return (ft_strjoin(stash, buf));
}

static int	read_buffer(int fd, char *buf, char **stash)
{
	ssize_t	bytes;

	bytes = read(fd, buf, BUFFER_SIZE);
	if (bytes < 0)
	{
		free(*stash);
		*stash = NULL;
		return (-1);
	}
	if (bytes == 0)
		return (0);
	buf[bytes] = '\0';
	*stash = append_to_stash(*stash, buf, bytes);
	if (!*stash)
		return (-1);
	return (1);
}

static char	*read_to_stash(int fd, char *stash)
{
	char	*buf;
	int		status;

	buf = malloc(BUFFER_SIZE + 1);
	if (!buf)
		return (NULL);
	status = 1;
	while (!ft_strchr(stash, '\n') && status > 0)
		status = read_buffer(fd, buf, &stash);
	free(buf);
	if (status == -1)
		return (NULL);
	return (stash);
}

char	*get_next_line(int fd)
{
	static char	*stash[1024];
	char		*line;
	char		*nl;

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);
	stash[fd] = read_to_stash(fd, stash[fd]);
	if (!stash[fd])
		return (NULL);
	line = extract_line(stash[fd]);
	nl = ft_strchr(stash[fd], '\n');
	if (!nl)
	{
		free(stash[fd]);
		stash[fd] = NULL;
	}
	else
	{
		nl = ft_substr(stash[fd], (nl - stash[fd]) + 1, ft_strlen(nl + 1));
		free(stash[fd]);
		stash[fd] = nl;
	}
	return (line);
}
