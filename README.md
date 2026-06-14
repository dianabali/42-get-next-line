*This project has been created as part of the 42 curriculum by dbali*

# get_next_line

## Description

`get_next_line` is a C function that reads one line at a time from a file descriptor.
A "line" is defined as a sequence of characters ending with `\n`, or the remaining
characters in the file if no final newline is present. The function is designed to be
called in a loop so that each successive call returns the next line, returning `NULL`
once the file is exhausted or an error occurs.

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
Each call to `get_next_line` appends new data to the stash until a `\n` is found, then
carves off exactly one line and saves whatever is left for the next call.

```
fd -> read() in chunks of BUFFER_SIZE
            |
        stash[fd] <- leftover from previous call append new data until '\n' found or EOF
            │       
            │
        extract_line() -> everything up to and including '\n'
            │
        returned to caller
            │
        trim_stash() -> everything after the '\n' saved back to stash[fd]
```

## Project Content
- `get_next_line.h` - prototypes of `get_next_line` and helpers. 
- `get_next_line.c` - core logic: `read_to_stash`, `extract_line`, `trim_stash`, `get_next_line`.
- `get_next_line_utils.c` - helper functions: `ft_strlen`, `ft_strchr`, `ft_strjoin`, `ft_substr`.


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
while ((line = get_next_line(0)) != NULL)
{
    printf("You typed: %s", line);
    free(line);
}
```

### Testing

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
./gnl  # modify main.c to pass fd=0 instead of open()
```
```
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

## Resources

- [`read()` man page](https://man7.org/linux/man-pages/man2/read.2.html) — reference for `read()` return values, `EINTR`, and EOF behaviour.
- [C static variables](https://en.cppreference.com/w/c/language/storage_duration) — covers storage duration, initialisation, and lifetime of `static` local variables.
- [File descriptors](https://www.codequoi.com/en/handling-a-file-by-its-descriptor-in-c/) — Chapters 4–5 explain the relationship between file descriptors, open file descriptions, and `read()`.

### Testers
- [gnlTester](https://github.com/Tripouille/gnlTester)
- [gnl-unit-test](https://github.com/harm-smits/gnl-unit-test)
