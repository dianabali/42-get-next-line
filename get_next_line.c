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

/*
	extract_line()
	Returns the complete line from the stash including the '\n' if one exists.
*/
static char	*extract_line(const char *stash)
{
	size_t	len; // Length of the line to extract
	char	*nl; // Pointer to the 1st new line

	// If no stash, there is no line to return
	if (!stash || stash[0] == '\0')
		return (NULL);
	nl = ft_strchr(stash, '\n');
	if (nl)
		len = (nl - stash) + 1; // If nl is found, give the number of chars +1 for '\n'
	else
		len = ft_strlen(stash); // Otherwise the entire stash is the final line
	return (ft_substr(stash, 0, len)); // Create and return a copy of the line
}

/*
	append_to_stash()
	Appends a newly read buffer to the existing stash
*/
static char	*append_to_stash(char *stash, char *buf, ssize_t bytes)
{
	// If stash doesn't exist yet, create a new string from the buffer
	if (!stash)
		return (ft_substr(buf, 0, bytes));
	// Otherwise concatenate stash + buffer
	return (ft_strjoin(stash, buf)); 
}

/*
	read_buffer()
	Reads BUFFER_SIZE bytes from the file and appends them to stash
	Return values:
		1 - successfully read data
		0 - end of file reached
		-1 - read error
*/
static int	read_buffer(int fd, char *buf, char **stash)
{
	ssize_t	bytes; // Number of bytes read

	bytes = read(fd, buf, BUFFER_SIZE); // Read up to BUFFER_SIZE bytes from the file

	// If read() fails
	if (bytes < 0)
	{
		free(*stash); // Free any stored data
		*stash = NULL; // Prevent dangling pointer
		return (-1); // Signal error
	}
	
	// If EOF reached
	if (bytes == 0)
		return (0);
	buf[bytes] = '\0';
	*stash = append_to_stash(*stash, buf, bytes);
	if (!*stash)
		return (-1);
	return (1);
}

/*
	read_to_stash()
	Reads until a newline is found, EOF is reached, or an error occurs.
*/
static char	*read_to_stash(int fd, char *stash)
{
	char	*buf; // Temporary read buffer
	int		status; // Status returned by read_buffer()

	buf = malloc(BUFFER_SIZE + 1);
	if (!buf)
		return (NULL);
	status = 1;

	// Continue reading until there is no newline and previous read succeeded
	while (!ft_strchr(stash, '\n') && status > 0)
		status = read_buffer(fd, buf, &stash);
	free(buf); // Free buf after reading
	if (status == -1)
		return (NULL);
	return (stash);
}

/*
	get_next_line()
	CORE LOGIC
	Returns one line each time it is called.
*/
char	*get_next_line(int fd)
{
	static char	*stash[1024]; // One stash for each fd
	char		*line; // Line returned to the caller
	char		*nl; // Pointer to '\n'

	// If invalid fd or invalid BUFFER_SIZE
	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);

	// Read until at least one complete line is available 
	stash[fd] = read_to_stash(fd, stash[fd]);

	// Reading failed
	if (!stash[fd])
		return (NULL);
	
	line = extract_line(stash[fd]); // Extract the fine from the stash
	nl = ft_strchr(stash[fd], '\n'); // Search for '\n'

	// No nl means this was the last line
	if (!nl)
	{
		free(stash[fd]);
		stash[fd] = NULL;
	}
	else
	{
		/* Keep only the text after the newline
		Example:
		Hello\nWorld
			- nl points to index 6
			- (nl - stash[fd]) gives the index of '\n'
			- +1 skips it
		*/
		nl = ft_substr(stash[fd], (nl - stash[fd]) + 1, ft_strlen(nl + 1));
		free(stash[fd]);
		stash[fd] = nl;
	}
	return (line);
}
