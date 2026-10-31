CC = cc
CFLAGS = -Wall -Wextra -Werror
SOURCES = ft_printf.c requiredft.c
OBJS = $(SOURCES:.c=.o)
NAME = libftprintf.a

all: $(NAME)

$(NAME): $(OBJS)
	ar -rcs $(NAME) $(OBJS)

%.o : %.c ft_printf.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: fclean clean all re
