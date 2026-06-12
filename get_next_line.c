#include "get_next_line.h"

/*
	extract_line
	Pulls everything up to and including the first '\n' (or the full stash if no '\n' present) as a new string.
	Parameters:
		- stash: the current accumulated string for this fd.
*/
static char	*extract_line(const char *stash)
{
	size_t	len; // length of the line to extract (up to and including '\n')
	char	*nl; // pointer to the first '\n' in stash

	if (!stash || stash[0] == '\0')
		return (NULL);
	nl = ft_strchr(stash, '\n');

	// If we found a newline, we want to include it in the line we return, so we add 1 to the length
	if (nl)
		len = (size_t)(nl - stash) + 1;
	// If no newline, we want to return the whole stash as the line
	else
		len = ft_strlen(stash);
	return (ft_substr(stash, 0, len)); // Extract the line from the stash
}

/*
	trim_stash
	Returns everything after the first '\n' in stash, or NULL if there is nothing left.
	Parameters:
		- stash: the current accumulated string for this fd.
*/
static char	*trim_stash(char *stash)
{
	char	*nl; // pointer to the first '\n' in stash
	char	*remainder; // the new stash string that will hold everything after the first '\n'

	nl = ft_strchr(stash, '\n');

	// If there is no newline, it means we've returned the whole stash as the line, so we can free it and return NULL for the new stash
	if (!nl)
	{
		free(stash);
		return (NULL);
	}

	// If there is a newline, we want to keep everything after it as the new stash for the next call
	// stash - the original string, 
	// (unsigned int)(nl - stash) + 1 - the index of the first character after the newline
	// ft_strlen(nl + 1) - the length of the remaining string
	remainder = ft_substr(stash, (unsigned int)(nl - stash) + 1, ft_strlen(nl + 1));
	free(stash); // free the old stash since we're replacing it with the remainder
	return (remainder);
}

/*
	read_to_stash
	Keeps reading fd into buf (BUFFER_SIZE bytes at a time) and appending to stash until we see a '\n' or reach EOF.
	Parameters:
		- fd: the file descriptor to read from.
		- stash: the current accumulated string for this fd (can be NULL on first call).
*/
static char	*read_to_stash(int fd, char *stash)
{
	char	buf[BUFFER_SIZE + 1]; // temporary buffer we read into, +1 for '\0'
	ssize_t	bytes; // number of bytes read in the last read() call

	// We keep reading until we find a newline in the stash
	while (!ft_strchr(stash, '\n'))
	{
		// Read up to BUFFER_SIZE bytes from fd into buf
		bytes = read(fd, buf, BUFFER_SIZE);
		// If read returns -1, there was an error, so we free the stash and return NULL
		if (bytes < 0)
		{
			free(stash);
			return (NULL);
		}
		// If read returns 0, we've reached EOF, so we break out of the loop (we might still have some data in stash to return)
		if (bytes == 0)
			break ;
		buf[bytes] = '\0';
		// If stash is NULL, we create a new string from buf
		if (!stash)
		{
			stash = ft_substr(buf, 0, (size_t)bytes);
			if (!stash)
				return (NULL);
		}
		// Otherwise, we append buf to the existing stash
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
	get_next_line: main function.

	Uses a static array of stash pointers indexed by fd so that up to
	OPEN_MAX file descriptors can be read concurrently without global vars.

	Algorithm:
		1. Read chunks of BUFFER_SIZE bytes, accumulating them in stash[fd].
		2. Stop reading as soon as a '\n' is found in stash (lazy reads).
		3. Carve the first line (up to and including '\n') out of the stash.
		4. Store whatever remains after that '\n' back into stash[fd].
		5. Return the carved line to the caller.
*/
char	*get_next_line(int fd)
{
	static char	*stash[1024]; // static array to hold the stash for each file descriptor (up to 1024)
	char		*line; // the line that will be extracted and returned to the caller

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);
	// Read from fd and accumulate in stash[fd] until we find a newline or reach EOF
	stash[fd] = read_to_stash(fd, stash[fd]);
	// If read_to_stash returns NULL, it means there was an error or we reached EOF without any data, so we return NULL
	if (!stash[fd])
		return (NULL);
	// Extract the next line from stash[fd]
	line = extract_line(stash[fd]);
	// Update stash[fd] to be everything after the extracted line
	stash[fd] = trim_stash(stash[fd]);
	return (line);
}
