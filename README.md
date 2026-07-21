*This project has been created as part of the 42 curriculum by dbali*

# get_next_line

### Description

`get_next_line` is a C function that reads one line at a time from a file descriptor.
A "line" is defined as a sequence of characters ending with `\n`, or the remaining
characters in the file if no final newline is present. The function is designed to be
called in a loop so that each successive call returns the next line, returning `NULL`
once the file is exhausted or an error occurs.

### Goal
The project's goal is to introduce two foundational C concepts:

- **Static variables** — the only mechanism (without globals) that lets a function
  preserve state between calls.
- **Dynamic memory management** — every returned line is heap-allocated and must be
  freed by the caller.

## Algorithm

### Why a "stash" (leftover buffer) approach?

`read()` does not know anything about newlines. It just fills a buffer with up to
`BUFFER_SIZE` raw bytes. A single `read()` call can therefore:

- land exactly on a `\n` (lucky),
- stop in the middle of a line (common when `BUFFER_SIZE` is small), or
- contain several lines at once (common when `BUFFER_SIZE` is large).

The solution is to keep a **persistent accumulator** (the *stash*) per file descriptor.
Each call to `get_next_line` appends new data to the stash until a `\n` is found, then carves off exactly one line and saves whatever is left for the next call.

### The static variable
```
static char *stash[1024];
```
A normal local variable dies when the function returns. A static local variable persists for the lifetime of the program keeping its value between calls. So when `get_next_line` returns a line to you and then gets called again, stash still holds whatever leftover data was sitting after that line.

The array has 1024 slots because static arrays have a size (OPEN_MAX) that holds one stash per file descriptor.
- stash[3] holds the leftover for fd 3 and so on.

## Project Content
- `get_next_line.h` - prototypes of `get_next_line()` and helper functions. 
- `get_next_line.c` - core logic: `read_to_stash()`, `extract_line()`, `trim_stash()`, `get_next_line()`.
- `get_next_line_utils.c` - helper functions: `ft_strlen`, `ft_strchr`, `ft_strjoin`, `ft_substr`.
- `get_next_line_bonus.h` - prototypes of `get_next_line_bonus()` and helper functions.
- `get_next_line_bonus.c` - core logic.
- `get_next_line_utils_bonus.c` - helper functions.

## Instructions

### Compilation

```bash
# With a buffer size (e.g. 42):
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c

# Without a buffer size:
cc -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c main.c
```

### Usage example

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    int   fd = open("test.txt", O_RDONLY);
    char *line;

    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return 0;
}
```

### Reading from stdin
```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    char *line;

    while ((line = get_next_line(0)) != NULL)
    {
        printf("You typed: %s", line);
        free(line);
    }
    return 0;
}
```

## Testing

```bash
# Buffer = 1
cc -Wall -Wextra -Werror -D BUFFER_SIZE=1  get_next_line.c get_next_line_utils.c main.c -o gnl
./gnl

# Buffer = 9999
cc -Wall -Wextra -Werror -D BUFFER_SIZE=9999 get_next_line.c get_next_line_utils.c main.c -o gnl
./gnl

# Empty file
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
touch empty.txt
./gnl

# File with no trailing newline
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
printf "hello\nworld\nno newline" > nonewline.txt
./gnl

# stdin (fd 0) — type lines, Ctrl + D to send EOF
cc -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
./gnl  # modify main.c to pass fd=0 instead of open() -> the example below
```
```c
int main(void)
{
    int   fd = 0;
    char *line;

    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    return 0;
}
```

## Checking memory leaks
```bash
cc -g -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
valgrind --leak-check=full --show-leak-kinds=all ./a.out

# Correct if you see this: 0 bytes in 0 blocks
```

## Read from multiple file descriptors
```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    int     fd1 = open("test.txt", O_RDONLY);
    int     fd2 = open("lorem_ipsum.txt", O_RDONLY);
    int     fd3 = open("charles.txt", O_RDONLY);
    char    *line;

    printf("fd1=%d  fd2=%d  fd3=%d\n\n", fd1, fd2, fd3);

    line = get_next_line(fd1);
    printf("fd1: %s", line);
    free(line);

    line = get_next_line(fd2);
    printf("fd2: %s", line);
    free(line);

    line = get_next_line(fd3);
    printf("fd3: %s", line);
    free(line);

    line = get_next_line(fd1);
    printf("fd1: %s", line);
    free(line);

    line = get_next_line(fd2);
    printf("fd2: %s", line);
    free(line);

    line = get_next_line(fd3);
    printf("fd3: %s", line);
    free(line);

    close(fd1);
    close(fd2);
    close(fd3);
    return (0);
}
```

## Resources

- [`read()` man page](https://man7.org/linux/man-pages/man2/read.2.html) — reference for `read()` return values, `EINTR`, and EOF behaviour.
- [C static variables](https://en.cppreference.com/w/c/language/storage_duration) — covers storage duration, initialisation, and lifetime of `static` local variables.
- [File descriptors](https://www.codequoi.com/en/handling-a-file-by-its-descriptor-in-c/) — chapters 4–5 explain the relationship between file descriptors, open file descriptions, and `read()`.

### Testers
- [gnlTester](https://github.com/Tripouille/gnlTester)

## Example of how this project works
File = "hi/nbye"

BUFFER_SIZE = 4.

First call:
```
stash[fd] = NULL  (first ever call)

read_to_stash:
    stash is NULL, no '\n' yet
    read() → "hi\nb"  (4 bytes)
    stash = "hi\nb"
    ft_strchr finds '\n' → exit loop
    return "hi\nb"

stash[fd] = "hi\nb"

extract_line("hi\nb"):
    nl points to the '\n' at index 2
    len = 2 + 1 = 3
    return ft_substr("hi\nb", 0, 3) = "hi\n"

trim_stash("hi\nb"):
    nl at index 2, remainder starts at index 3
    ft_substr("hi\nb", 3, 1) = "b"
    free("hi\nb")
    return "b"

stash[fd] = "b"
return "hi\n"  ← caller receives this
```

Second call:
```
stash[fd] = "b"

read_to_stash:
    no '\n' in "b"
    read() → "ye"  (2 bytes, EOF reached)
    stash = ft_strjoin("b", "ye") = "bye"
    read() → 0 bytes (EOF) → break
    return "bye"

extract_line("bye"):
    no '\n', take everything
    return "bye"

trim_stash("bye"):
    no '\n' → free("bye"), return NULL

stash[fd] = NULL
return "bye"  ← caller receives this
```

Third call:
```
stash[fd] = NULL
read_to_stash → read() returns 0 immediately (EOF)
stash stays NULL
get_next_line returns NULL  ← signals end of file
```
