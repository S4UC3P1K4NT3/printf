/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   requiredft.c                                      :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: gtomasin <gtomasin@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/31 17:17:01 by gtomasin         #+#    #+#              */
/*   Updated: 2026/10/31 17:37:58 by gtomasin        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putnbr_fd(int nb, int fd)
{
	char	c;
	int		len;

	len = 0;
	if (nb == -2147483648)
		return (write(fd, "-2147483648", 11), 11);
	if (nb < 0)
	{
		len += write(fd, "-", 1);
		nb = -nb;
	}
	if (nb >= 10)
		len += ft_putnbr_fd(nb / 10, fd);
	c = nb % 10 + '0';
	len += write(fd, &c, 1);
	return (len);
}

int	ft_putchar_fd(char c, int fd)
{
	return (write(fd, &c, 1));
}

int	ft_putstr_fd(char *s, int fd)
{
	return (write(fd, s, ft_strlen(s)));
}

int	ft_puthex_fd(int nb, int fd)
{
	char	*base;
	char	c;
	int		len;

	base = "0123456789abcdef";
	len = 0;
	if (nb == -2147483648)
		return (write(fd, "-2147483648", 11), 11);
	if (nb < 0)
	{
		len += write(fd, "-", 1);
		nb = -nb;
	}
	if (nb >= 16)
		len += ft_puthex_fd(nb / 16, fd);
	c = base[nb % 16];
	len += write(fd, &c, 1);
	return (len);
}

int	ft_putheX_fd(int nb, int fd)
{
	char	*base;
	char	c;
	int		len;

	base = "0123456789ABCDEF";
	len = 0;
	if (nb == -2147483648)
		return (write(fd, "-2147483648", 11), 11);
	if (nb < 0)
	{
		len += write(fd, "-", 1);
		nb = -nb;
	}
	if (nb >= 16)
		len += ft_putheX_fd(nb / 16, fd);
	c = base[nb % 16];
	len += write(fd, &c, 1);
	return (len);
}
