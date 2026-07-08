#include "get_next_line.h"

/*
	ft_strlen
	Returns the length of the string s, or 0 if s is NULL.
	Parameters:
		- s: the string to measure.
*/
size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}

/*
	ft_strchr
	Returns a pointer to the first occurrence of character c in string s, or NULL if c is not found. 
	If c is '\0', returns a pointer to the null terminator.
	Parameters:
		- s: the string to search.
		- c: the character to find.
*/
char	*ft_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
	while (*s)
	{
		if ((unsigned char)*s == (unsigned char)c)
			return ((char *)s);
		s++;
	}
	if ((unsigned char)c == '\0')
		return ((char *)s);
	return (NULL);
}

/*
	ft_strjoin
	Joins s1 and s2 into a new string, then frees s1.
	Returns NULL and frees s1 on allocation failure.
	Parameters:
		- s1: the stash that contains everything accumulated so far.
		- s2: a new chunk read from the file, to be appended to the stash.
*/
char	*ft_strjoin(char *s1, const char *s2)
{
	char	*result;
	size_t	len1;
	size_t	len2;
	size_t	i;

	// Calculate lengths of s1 and s2 so we know how much memory to allocate
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	result = malloc(len1 + len2 + 1);

	// If malloc fails, free s1 to prevent memory leak and return NULL
	if (!result)
	{
		free(s1);
		return (NULL);
	}
	i = 0;

	// Copy s1 into result
	while (i < len1)
	{
		result[i] = s1[i];
		i++;
	}

	// Append s2 to result
	while (i < len1 + len2)
	{
		result[i] = s2[i - len1]; // i - len1 because we want to start from the beginning of s2. You can also do `size_t j = 0` where j is the index of s2
		i++;
	}
	result[i] = '\0';
	free(s1); // Free s1 after joining to prevent memory leak
	return (result);
}

/*
	ft_substr
	Returns a substring of s starting at index 'start', at most 'len' bytes.
	Parameters:
		- s: the original string.
		- start: the index to start the substring from.
		- len: the maximum length of the substring.
*/
char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char	*sub; // The substring to return
	size_t	slen; // The length of s
	size_t	i;

	if (!s)
		return (NULL);
	slen = ft_strlen(s); // Calculate the length of s

	// If start is beyond the end of s, return an empty string
	if (start >= (unsigned int)slen)
		len = 0;
	
	// If len goes beyond the end of s, adjust it to fit within s
	else if (len > slen - start)
		len = slen - start;
	sub = malloc(len + 1);
	if (!sub)
		return (NULL);
	i = 0;
	while (i < len)
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}
