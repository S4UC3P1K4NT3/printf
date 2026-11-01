/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   requiredft.c                                      :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: gtomasin <gtomasin@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/31 17:17:01 by gtomasin         #+#    #+#              */
/*   Updated: 2026/11/02 00:05:51 by gtomasin        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putnbr_fd(int nb, int fd)
{
	char	c;
	int		len;

	len = 0;
	if (nb == -2147483648)
		return (write(fd, "-2147483648", 11));
	if (nb < 0)
	{
		len += write(fd, "-", 1);
		nb = -nb;
	}
	if (nb >= 10)
		len += ft_putnbr_fd(nb / 10, fd);
	c = nb % 10 + '0';
	if (write(fd, &c, 1) < 0)
		return (-1);
	len++;
	return (len);
}

int	ft_putchar_fd(char c, int fd)
{
	return (write(fd, &c, 1));
}

int	ft_putstr_fd(char *s, int fd)
{
	int	len;

	if (!s)
		return (write(fd, "(null)", 6));
	len = 0;
	while (*s)
	{
		if (write(fd, s, 1))
			return (-1);
		len++;
		s++;
	}
	return (len);
}

int	ft_puthex(unsigned long nb, char *base)
{
	char	c;
	int		len;

	len = 0;
	if (nb >= 16)
		len += ft_puthex(nb / 16, base);
	c = base[nb % 16];
	if (write(1, &c, 1) < 0)
		return (-1);
	len++;
	return (len);
}

int	ft_putunbr(unsigned int nb)
{
	char	c;
	int		len;

	len = 0;
	if (nb >= 10)
		len += ft_putunbr(nb / 10);
	c = nb % 10 + '0';
	if (write(1, &c, 1) < 0)
		return (-1);
	len++;
	return (len);
}
