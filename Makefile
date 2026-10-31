CC = cc
CFLAGS = -Wall -Wextra -Werror
SOURCES = ft_printf.c requiredft.c
OBJS = $(SOURCES:.c=.o)
TARGET = libftprintf.a

all: $(TARGET)

$(TARGET): $(OBJS)
	ar -rcs $(TARGET) $(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: fclean clean all re
