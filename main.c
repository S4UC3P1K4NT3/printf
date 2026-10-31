/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   main.c                                            :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: gtomasin <gtomasin@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/31 19:01:23 by gtomasin         #+#    #+#              */
/*   Updated: 2026/10/31 19:04:10 by gtomasin        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <fcntl.h>
#include "ft_printf.h"

#ifndef O_BINARY
# define O_BINARY 0
#endif

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"
#define TMP_FILE "ft_test_out.tmp"

static int	g_ok = 0;
static int	g_ko = 0;

static int	start_capture(void)
{
	int	saved;
	int	fd;

	fflush(stdout);
	saved = dup(1);
	fd = open(TMP_FILE, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0644);
	dup2(fd, 1);
	close(fd);
	return (saved);
}

static void	stop_capture(int saved)
{
	dup2(saved, 1);
	close(saved);
}

static void	check(const char *label, const char *exp, int exp_len, int ret)
{
	char	got[256];
	int		got_len;
	FILE	*f;

	got_len = 0;
	f = fopen(TMP_FILE, "rb");
	if (f)
	{
		got_len = (int) fread(got, 1, sizeof(got) - 1, f);
		fclose(f);
	}
	got[got_len] = '\0';
	if (got_len == exp_len && memcmp(got, exp, exp_len) == 0
			&& ret == exp_len)
	{
		printf(GREEN "%s : ok" RESET "\n", label);
		g_ok++;
	}
	else
	{
		printf(RED "%s : KO" RESET "\n", label);
		printf("    attendu : \"%s\" (ret %d)\n", exp, exp_len);
		printf("    obtenu  : \"%s\" (ret %d)\n", got, ret);
		g_ko++;
	}
}

/* Calcule ce que %p doit afficher pour une adresse (format Linux) */

static void	ptr_expected(void *p, char *buf)
{
	uintptr_t	v;
	char		tmp[32];
	int			i;
	int			j;

	v = (uintptr_t) p;
	i = 0;
	if (v == 0)
		tmp[i++] = '0';
	while (v)
	{
		tmp[i++] = "0123456789abcdef"[v % 16];
		v /= 16;
	}
	buf[0] = '0';
	buf[1] = 'x';
	j = 2;
	while (i > 0)
		buf[j++] = tmp[--i];
	buf[j] = '\0';
}

#define TEST(exp, ...) do { int s_ = start_capture(); int r_ = ft_printf(__VA_ARGS__); stop_capture(s_); check(#__VA_ARGS__, exp, (int)(sizeof(exp) - 1), r_); } while (0)

int	main(void)
{
	int		x;
	int		saved;
	int		ret;
	char	buf[64];

	printf("===== %%c =====\n");
	TEST("A", "%c", 'A');
	TEST("\0", "%c", '\0');
	printf("===== %%s =====\n");
	TEST("hello", "%s", "hello");
	TEST("", "%s", "");
	TEST("(null)", "%s", (char *) NULL);
	printf("===== %%d / %%i =====\n");
	TEST("0", "%d", 0);
	TEST("-42", "%d", -42);
	TEST("-2147483648", "%d", INT_MIN);
	TEST("2147483647", "%i", INT_MAX);
	printf("===== %%u =====\n");
	TEST("0", "%u", 0u);
	TEST("42", "%u", 42u);
	TEST("4294967295", "%u", UINT_MAX);
	printf("===== %%x / %%X =====\n");
	TEST("0", "%x", 0u);
	TEST("ff", "%x", 255u);
	TEST("FF", "%X", 255u);
	TEST("ffffffff", "%x", UINT_MAX);
	TEST("FFFFFFFF", "%X", UINT_MAX);
	TEST("ffffffff", "%x", -1);
	printf("===== %%p =====\n");
	TEST("(nil)", "%p", (void *) NULL);
	ptr_expected(&x, buf);
	saved = start_capture();
	ret = ft_printf("%p", &x);
	stop_capture(saved);
	check("\"%p\", &x", buf, (int) strlen(buf), ret);
	printf("===== %%%% et texte =====\n");
	TEST("%", "%%");
	TEST("texte simple", "texte simple");
	TEST("", "");
	TEST("a1bxc", "a%db%sc", 1, "x");
	TEST("zok-4242ffFF%", "%c%s%d%u%x%X%%", 'z', "ok", -42, 42u, 255u, 255u);
	remove(TMP_FILE);
	printf("\n%s%d ok" RESET " / %s%d KO" RESET "\n",
		GREEN, g_ok, g_ko ? RED : GREEN, g_ko);
	return (g_ko != 0);
}
