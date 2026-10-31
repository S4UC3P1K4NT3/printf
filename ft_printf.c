/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   ft_printf.c                                       :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: gtomasin <gtomasin@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/31 16:52:40 by gtomasin         #+#    #+#              */
/*   Updated: 2026/10/31 21:22:41 by gtomasin        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_putptr(void *ptr)
{
	int	len;
	int	ret;

	if (!ptr)
		return (ft_putstr_fd("(nil)", 1));
	len = ft_putstr_fd("0x", 1);
	if (len < 0)
		return (-1);
	ret = ft_puthex((unsigned long) ptr, "0123456789abcdef");
	if (ret < 0)
		return (-1);
	return (len + ret);
}

static int	getvalue(char c, va_list args)
{
	if (c == 'c')
		return (ft_putchar_fd(va_arg(args, int), 1));
	else if (c == 's')
		return (ft_putstr_fd(va_arg(args, char *), 1));
	else if (c == 'd' || c == 'i')
		return (ft_putnbr_fd(va_arg(args, int), 1));
	else if (c == '%')
		return (ft_putchar_fd('%', 1));
	else if (c == 'u')
		return (ft_putunbr(va_arg(args, unsigned int)));
	else if (c == 'x')
		return (ft_puthex(va_arg(args, unsigned int), "0123456789abcdef"));
	else if (c == 'X')
		return (ft_puthex(va_arg(args, unsigned int), "0123456789ABCDEF"));
	else if (c == 'p')
		return (ft_putptr(va_arg(args, void *)));
	return (-1);
}

int	ft_printf(const char *str, ...)
{
	va_list	args;
	int		i;
	int		len;
	int		tmp;

	if (!str)
		return (-1);
	len = 0;
	i = -1;
	va_start(args, str);
	while (str[++i])
	{
		if (str[i] == '%')
			tmp = getvalue(str[++i], args);
		else
			tmp = ft_putchar_fd(str[i], 1);
		if (tmp < 0)
			return (va_end(args), -1);
		len += tmp;
	}
	va_end(args);
	return (len);
}

// #include <stdio.h>
// #include <limits.h>

// int	main(void)
// {
// 	int	x;
// 	int	r1;
// 	int	r2;

// 	r1 = ft_printf("[%c|%s|%d|%i]\n", 'a', "abc", INT_MIN, INT_MAX);
// 	r2 = printf("[%c|%s|%d|%i]\n", 'a', "abc", INT_MIN, INT_MAX);
// 	printf("ret %d / %d\n\n", r1, r2);
// 	r1 = ft_printf("[%u|%x|%X|%%]\n", UINT_MAX, 255u, 255u);
// 	r2 = printf("[%u|%x|%X|%%]\n", UINT_MAX, 255u, 255u);
// 	printf("ret %d / %d\n\n", r1, r2);
// 	r1 = ft_printf("[%p|%p]\n", (void *)&x, (void *)0);
// 	r2 = printf("[%p|%p]\n", (void *)&x, (void *)0);
// 	printf("ret %d / %d\n\n", r1, r2);
// 	r1 = ft_printf("[%s]\n", (char *)0);
// 	printf("ret %d (attendu : [(null)] et 9)\n", r1);
// 	return (0);
// }
