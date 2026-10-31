/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   main.c                                            :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: gtomasin <gtomasin@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/31 19:11:56 by gtomasin         #+#    #+#              */
/*   Updated: 2026/10/31 19:11:56 by gtomasin        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "ft_printf.h"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"
#define NAME_W 40
#define BUF_SIZE 65536
#define REAL_FILE ".test_real.tmp"
#define FT_FILE ".test_ft.tmp"

static char		g_exp[BUF_SIZE];
static char		g_got[BUF_SIZE];
static int		g_exp_len;
static int		g_exp_ret;
static int		g_saved;
static int		g_pipe[2];
static pid_t	g_pid;
static int		g_ok;
static int		g_ko;

static int	read_file(const char *path, char *buf)
{
	int	fd;
	int	n;
	int	total;

	total = 0;
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	while (total < BUF_SIZE)
	{
		n = read(fd, buf + total, BUF_SIZE - total);
		if (n <= 0)
			break ;
		total += n;
	}
	close(fd);
	return (total);
}

/* ---------- capture du vrai printf ---------- */

static void	cap_begin(void)
{
	int	fd;

	fflush(stdout);
	g_saved = dup(1);
	fd = open(REAL_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	dup2(fd, 1);
	close(fd);
}

static void	cap_end(int ret)
{
	fflush(stdout);
	dup2(g_saved, 1);
	close(g_saved);
	g_exp_ret = ret;
	g_exp_len = read_file(REAL_FILE, g_exp);
}

static void	set_expected(const char *s, int len, int ret)
{
	memcpy(g_exp, s, len);
	g_exp_len = len;
	g_exp_ret = ret;
}

/* ---------- execution de ft_printf dans un processus fils ---------- */

static int	child_begin(void)
{
	int	fd;

	fflush(stdout);
	if (pipe(g_pipe) < 0)
	{
		perror("pipe");
		exit(2);
	}
	g_pid = fork();
	if (g_pid < 0)
	{
		perror("fork");
		exit(2);
	}
	if (g_pid == 0)
	{
		close(g_pipe[0]);
		fd = open(FT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		dup2(fd, 1);
		close(fd);
		alarm(3);
		return (1);
	}
	close(g_pipe[1]);
	return (0);
}

static void	child_end(int ret)
{
	if (write(g_pipe[1], &ret, sizeof(ret)) < 0)
		_exit(1);
	_exit(0);
}

static int	collect(int *ret, int *len)
{
	int	status;
	int	n;

	*ret = 0;
	waitpid(g_pid, &status, 0);
	n = read(g_pipe[0], ret, sizeof(*ret));
	close(g_pipe[0]);
	*len = read_file(FT_FILE, g_got);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
		return (2);
	if (WIFSIGNALED(status) || n != (int) sizeof(*ret))
		return (1);
	return (0);
}

/* ---------- affichage ---------- */

static void	pass(void)
{
	printf(GREEN "ok" RESET "\n");
	g_ok++;
}

static void	fail(const char *why)
{
	printf(RED "%s" RESET "\n", why);
	g_ko++;
}

static void	section(const char *title)
{
	printf(BOLD "\n[ %s ]\n" RESET, title);
}

static void	show(const char *s, int len)
{
	int				i;
	unsigned char	c;

	printf("\"");
	i = 0;
	while (i < len && i < 60)
	{
		c = (unsigned char) s[i];
		if (c == '\n')
			printf("\\n");
		else if (c == '\t')
			printf("\\t");
		else if (c < 32 || c >= 127)
			printf("\\x%02x", c);
		else
			printf("%c", c);
		i++;
	}
	if (len > 60)
		printf("...");
	printf("\"");
}

static int	start_line(const char *name, int *ret, int *len)
{
	int	st;

	st = collect(ret, len);
	printf("  %-*s", NAME_W, name);
	if (st == 1)
		fail("CRASH");
	else if (st == 2)
		fail("TIMEOUT (boucle infinie ?)");
	return (st);
}

static void	report(const char *name)
{
	int	ret;
	int	len;

	if (start_line(name, &ret, &len))
		return ;
	if (ret == g_exp_ret && len == g_exp_len
			&& memcmp(g_got, g_exp, len) == 0)
	pass();
	else
	{
		fail("KO");
		printf("      attendu : ");
		show(g_exp, g_exp_len);
		printf("  (ret %d)\n      obtenu  : ", g_exp_ret);
		show(g_got, len);
		printf("  (ret %d)\n", ret);
	}
}

static void	report_survive(const char *name)
{
	int	ret;
	int	len;

	if (start_line(name, &ret, &len) == 0)
		pass();
}

static void	report_ret(const char *name, int expected)
{
	int	ret;
	int	len;

	if (start_line(name, &ret, &len))
		return ;
	if (ret == expected)
		pass();
	else
	{
		fail("KO");
		printf("      attendu : ret %d\n      obtenu  : ret %d\n",
			expected, ret);
	}
}

/* T : compare ft_printf au vrai printf (affichage + retour) */
#define T(name, fmt, ...) do { const char *f_ = (fmt); int r_; cap_begin(); r_ = printf(f_, ##__VA_ARGS__); cap_end(r_); if (child_begin()) child_end(ft_printf(f_, ##__VA_ARGS__)); report(name); } while (0)
/* R : verifie seulement que ft_printf ne plante pas */
#define R(name, fmt, ...) do { const char *f_ = (fmt); if (child_begin()) child_end(ft_printf(f_, ##__VA_ARGS__)); report_survive(name); } while (0)

int	main(void)
{
	int			x;
	void		*heap;
	static char	longstr[10001];
	static char	longfmt[5001];

	x = 42;
	heap = malloc(1);
	memset(longstr, 'x', 10000);
	memset(longfmt, 'y', 5000);
	section("%c");
	T("lettre", "%c", 'A');
	T("caractere nul", "%c", '\0');
	T("caractere nul entre du texte", "a%cb", '\0');
	T("espace", "%c", ' ');
	T("retour a la ligne", "%c", '\n');
	T("caractere 127 (DEL)", "%c", 127);
	T("caractere 255 (non ASCII)", "%c", 255);
	T("valeur negative (-1)", "%c", -1);
	T("trois caracteres colles", "%c%c%c", 'a', 'b', 'c');
	section("%s");
	T("chaine simple", "%s", "hello");
	T("chaine vide", "%s", "");
	T("pointeur NULL", "%s", (char *) NULL);
	T("NULL entre du texte", "a%sb", (char *) NULL);
	T("caracteres speciaux", "%s", "\t\v\f\r\n");
	T("UTF-8 / non ASCII", "%s", "h\xc3\xa9llo \xe2\x82\xac");
	T("chaine contenant des %", "%s", "%d %s %%");
	T("chaine de 10000 caracteres", "%s", longstr);
	T("deux chaines collees", "%s%s", "abc", "def");
	T("chaine vide puis NULL", "%s%s", "", (char *) NULL);
	section("%d / %i");
	T("zero", "%d", 0);
	T("un", "%d", 1);
	T("moins un", "%d", -1);
	T("9 et 10", "%d %d", 9, 10);
	T("-9 et -10", "%d %d", -9, -10);
	T("puissances de 10", "%d %d %d", 100, 1000, 1000000000);
	T("INT_MAX", "%d", INT_MAX);
	T("INT_MIN", "%d", INT_MIN);
	T("INT_MIN + 1", "%d", INT_MIN + 1);
	T("%i positif", "%i", 42);
	T("%i zero", "%i", 0);
	T("%i INT_MIN", "%i", INT_MIN);
	T("%d et %i colles", "%d%i", -42, 42);
	T("limites ensemble", "%d %i %d", INT_MIN, 0, INT_MAX);
	section("%u");
	T("zero", "%u", 0u);
	T("un", "%u", 1u);
	T("INT_MAX", "%u", (unsigned int) INT_MAX);
	T("INT_MAX + 1", "%u", (unsigned int) INT_MAX + 1u);
	T("UINT_MAX", "%u", UINT_MAX);
	T("-1 (devient UINT_MAX)", "%u", -1);
	T("INT_MIN", "%u", INT_MIN);
	T("4000000000", "%u", 4000000000u);
	T("deux %u colles", "%u%u", 1u, UINT_MAX);
	section("%x / %X");
	T("zero", "%x", 0u);
	T("1 et 9", "%x %x", 1u, 9u);
	T("10 (a / A)", "%x %X", 10u, 10u);
	T("15 et 16", "%x %x", 15u, 16u);
	T("255 et 256", "%x %x", 255u, 256u);
	T("0xdeadbeef minuscules", "%x", 0xdeadbeefu);
	T("0xDEADBEEF majuscules", "%X", 0xdeadbeefu);
	T("UINT_MAX", "%x %X", UINT_MAX, UINT_MAX);
	T("-1", "%x", -1);
	T("INT_MIN", "%X", INT_MIN);
	T("INT_MAX", "%x", INT_MAX);
	T("%x et %X colles", "%x%X", 0xabcu, 0xabcu);
	section("%p");
	T("NULL", "%p", (void *) NULL);
	T("adresse sur la pile", "%p", (void *) & x);
	T("adresse sur le tas (malloc)", "%p", heap);
	T("adresse d'une chaine litterale", "%p", (void *) "abc");
	T("adresse 1", "%p", (void *) 1);
	T("adresses 15 et 16", "%p %p", (void *) 15, (void *) 16);
	T("LONG_MAX", "%p", (void *) LONG_MAX);
	T("LONG_MIN", "%p", (void *) LONG_MIN);
	T("ULONG_MAX", "%p", (void *) ULONG_MAX);
	T("NULL puis adresse", "%p %p", (void *) NULL, (void *) & x);
	section("%%");
	T("pourcent seul", "%%");
	T("deux pourcents", "%%%%");
	T("%%d affiche %d", "%%d");
	T("100%%", "100%%");
	T("%% puis %d", "%%%d", 42);
	T("%d puis %%", "%d%%", 42);
	T("%%c%%s%%p", "%%c%%s%%p");
	section("texte et format");
	T("format vide", "");
	T("texte sans conversion", "Hello 42!");
	T("retour a la ligne seul", "\n");
	T("\\0 au milieu du format", "ab\0cd");
	T("format de 5000 caracteres", longfmt);
	T("tabulations et retours", "\t%d\n\t%s\n", 1, "a");
	section("melanges");
	T("toutes les conversions", "%c|%s|%d|%i|%u|%x|%X|%p|%%", 'z', "ok", -42, 42, 42u, 255u, 255u, (void *) & x);
	T("toutes les limites", "%d %d %u %x %X %p %p %s", INT_MIN, INT_MAX, UINT_MAX, UINT_MAX, 0u, (void *) NULL, (void *) ULONG_MAX, (char *) NULL);
	T("20 arguments", "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
	T("conversions sans separateur", "%s%d%s%x", "a", 1, "b", 255u);
	T("texte autour de tout", "[%c] [%s] [%d] [%p]", 'a', "b", 3, (void *) NULL);
	section("comportements internes");
	set_expected("abcde", 5, 2);
	if (child_begin())
	{
		ft_printf("abc");
		child_end(ft_printf("de"));
	}
	report("deux appels successifs (static)");
	set_expected("abc", 3, 1);
	if (child_begin())
	{
		ft_printf("a");
		if (write(1, "b", 1) < 0)
			_exit(1);
		child_end(ft_printf("c"));
	}
	report("pas de buffer interne");
	if (child_begin())
	{
		close(1);
		child_end(ft_printf("abc %d %s", 42, "xyz"));
	}
	report_ret("sortie fermee -> retourne -1", -1);
	section("robustesse (hors sujet, ne doit pas planter)");
	R("% en fin de chaine", "abc%");
	R("% seul", "%");
	R("conversion inconnue %y", "%y");
	R("drapeau non gere %5d", "%5d", 42);
	R("espace apres % (% d)", "% d", 42);
	R("format NULL", NULL);
	free(heap);
	unlink(REAL_FILE);
	unlink(FT_FILE);
	printf(BOLD "\nResultat : " RESET);
	if (g_ko == 0)
		printf(GREEN "%d/%d ok" RESET "\n", g_ok, g_ok);
	else
		printf("%d ok, " RED "%d KO" RESET " sur %d\n",
		g_ok, g_ko, g_ok + g_ko);
	return (g_ko != 0);
}
