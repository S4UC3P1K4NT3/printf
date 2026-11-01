*This project has been created as part of the 42 curriculum by gtomasin.*

# ft_printf

## Description

**ft_printf** is a re-implementation of the C standard library function `printf()`.
The goal of the project is to learn how **variadic functions** work in C
(`va_start`, `va_arg`, `va_end`) and to build a small, reusable static library,
`libftprintf.a`, that can be linked into future 42 projects.

The function has the following prototype:

```c
int	ft_printf(const char *format, ...);
```

It writes the formatted output to the standard output and returns the number of
characters printed, or `-1` if an error occurs.

### Supported conversions

| Conversion | Description                                          |
|------------|------------------------------------------------------|
| `%c`       | A single character                                   |
| `%s`       | A string (`(null)` if the pointer is `NULL`)         |
| `%p`       | A pointer address in hexadecimal, prefixed by `0x` (`(nil)` if `NULL`, as on Linux) |
| `%d`       | A signed decimal integer                             |
| `%i`       | A signed integer in base 10                          |
| `%u`       | An unsigned decimal integer                          |
| `%x`       | An unsigned integer in lowercase hexadecimal         |
| `%X`       | An unsigned integer in uppercase hexadecimal         |
| `%%`       | A literal percent sign                               |

Unlike the real `printf`, `ft_printf` does **not** use an internal buffer:
every character is written immediately with `write(2)`.

## Instructions

### Requirements

- A C compiler (`cc`, `gcc` or `clang`)
- `make`
- A Unix-like system (Linux recommended; the expected outputs follow glibc's `printf`)

### Compilation

At the root of the repository:

```bash
make        # builds libftprintf.a
make clean  # removes object files
make fclean # removes object files and libftprintf.a
make re     # fclean + all
```

The sources are compiled with `-Wall -Wextra -Werror`.

### Usage

Include the header in your code:

```c
#include "ft_printf.h"

int	main(void)
{
	int	n;

	n = ft_printf("Hello %s, you are %d years old (%x in hex)\n", "42", 21, 21);
	ft_printf("%d characters printed\n", n);
	return (0);
}
```

Then compile it with the library:

```bash
cc -Wall -Wextra -Werror main.c -L. -lftprintf -o program
./program
```

### Project structure

```
.
├── Makefile
├── ft_printf.h     # prototypes and includes
├── ft_printf.c     # main loop, conversion dispatch, %p handling
└── requiredft.c    # low-level output helpers (char, string, numbers, hex)
```

## Algorithm and data structure

### Overview

`ft_printf` relies on a **single linear scan** of the format string, combined
with a **dispatch function** and **recursive number printing**.

```
for each character of format:
    if it is '%':
        read the next character (the conversion specifier)
        dispatch to the matching printing function
    else:
        write the character as-is
    if the write failed:
        stop and return -1
    add the number of written characters to the total
return the total
```

### 1. Parsing: a single pass over the format string

The format string is read once, from left to right, with an index `i`.
When a `%` is found, the index moves to the next character, which identifies
the conversion. The loop's own increment then skips it.

- **Complexity:** O(n), where n is the length of the format plus the length of the printed output.
- **Why:** the mandatory part has no flags, width or precision, so there is
  nothing to look ahead for. A single pass is the simplest and fastest option,
  and it needs no extra memory.

If `format` is `NULL`, the function returns `-1` right away instead of crashing.

### 2. Dispatch: an `if / else if` chain

The function `getvalue()` receives the specifier and the `va_list`. It fetches
the argument with the correct type (`int`, `unsigned int`, `char *`, `void *`)
and calls the matching helper. An unknown specifier returns `-1`.

- **Why not an array of function pointers?** With only 9 conversions, the
  arguments have different types, and each one must be read with a different
  `va_arg` call. A lookup table would still need one wrapper per type, so it
  would add complexity without any real benefit. The `if` chain is short,
  readable and easy to check during evaluation.

### 3. Number conversion: recursion with a base string

Numbers are printed with **recursive functions**:

```c
int	ft_puthex(unsigned long nb, char *base)
{
	if (nb >= 16)
		len += ft_puthex(nb / 16, base);   /* print the higher digits first */
	write(1, &base[nb % 16], 1);           /* then the last digit */
}
```

- The **base string** (`"0123456789abcdef"` or `"0123456789ABCDEF"`) works as a
  lookup table: `base[nb % 16]` gives the digit directly. The same function
  therefore handles `%x`, `%X` and `%p`, and only the string changes.
- Recursion prints digits **in the correct order** without needing a temporary
  buffer to reverse them.
- The **recursion depth is bounded and small**: at most 10 calls for a 32-bit
  decimal value and 16 calls for a 64-bit hexadecimal address. There is no risk
  of stack overflow.

**Type choices:**

- `%d` / `%i` use an `int`. `INT_MIN` is handled as a special case, because
  `-INT_MIN` does not fit in an `int`.
- `%u` uses a dedicated `unsigned int` function, so that values above
  `INT_MAX` are not printed as negative numbers.
- `%x`, `%X` and `%p` share a function that takes an `unsigned long`. That type
  is large enough for a 64-bit address on Linux, and it never needs a sign.

### 4. Output and error handling: no buffer

Every character is written immediately with `write(2)`. Each helper returns
the number of characters written, or a negative value if `write` fails.
The main loop stores every result in a temporary variable: if it is negative,
`va_end` is called and `ft_printf` returns `-1`, like the real `printf`.

- **Why no buffer?** A buffer would require either `malloc` (so error and
  memory-leak handling) or a fixed array with flush logic. It would also change
  the output order when `ft_printf` is mixed with direct `write` calls.
  Writing directly keeps the code simple, and the library uses **no dynamic
  memory at all**, so it cannot leak.
- **Trade-off:** one system call per character is slower than a buffered
  `printf`. That cost is acceptable for this project, where correctness and
  simplicity matter most.

### Data structures used

| Structure      | Role                                                              |
|----------------|-------------------------------------------------------------------|
| `va_list`      | Iterates over the variable arguments, in order                    |
| `const char *` | The format string, read once through an index                     |
| Base strings   | Constant lookup tables that map a value from 0 to 15 to its digit |
| Call stack     | Stores the pending digits during recursive number printing        |

No heap allocation, no linked list and no global or `static` variables are
used, so each call to `ft_printf` is independent of the previous ones.

## Resources

### Documentation

- [`printf(3)` – Linux manual page](https://man7.org/linux/man-pages/man3/printf.3.html)
- [`stdarg(3)` – Linux manual page](https://man7.org/linux/man-pages/man3/stdarg.3.html)
- [`write(2)` – Linux manual page](https://man7.org/linux/man-pages/man2/write.2.html)
- [cppreference – `printf` family](https://en.cppreference.com/w/c/io/fprintf)
- [cppreference – Variadic functions](https://en.cppreference.com/w/c/variadic)
- The 42 Norm (`norminette`) and the ft_printf subject

### Testing tools

- [printfTester](https://github.com/Tripouille/printfTester)
- `valgrind` to check memory errors
- A personal test script that compares `ft_printf` with the real `printf`
  (output and return value) on edge cases

### Use of AI

An AI assistant (Claude) was used to generate this `README.md` file,
to review my code and point out bugs (which I then fixed myself),
and to write test scripts. It was not used to write the project's source code.

