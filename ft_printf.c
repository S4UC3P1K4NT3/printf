/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   ft_printf.c                                       :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: gtomasin <gtomasin@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/31 16:52:40 by gtomasin         #+#    #+#              */
/*   Updated: 2026/10/31 17:37:15 by gtomasin        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf(const char *str, ...)
{
	va_list	args;
	int		i;
	int		len;

	len = 0;
	i = 0;
	va_start(args, str);
	while (str[i])
	{
		if (str[i] == '%')
		{
			len += getvalue(str[i + 1], args);
			i++;
		}
		else
			len += ft_putchar_fd(str[i], 1);
		i++;
	}
	va_end(args);
	return (len);
}

int	getvalue(char c, va_list args)
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
		return (ft_putnbr_fd(va_arg(args, unsigned int), 1));
	else if (c == 'x')
		return (ft_puthex_fd(va_arg(args, unsigned int), 1));
	else if (c == 'X')
		return (ft_putheX_fd(va_arg(args, unsigned int), 1));
	return (0);
}
