#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"
#include <ctype.h>
#include <limits.h>
#include <errno.h>

static int is_valid_number(const char *str)
{
	char *trimmed;
	char *endptr;
	int i, j;
	
	if (!str)
		return (0);
	
	trimmed = ft_strdup((char *)str);
	if (!trimmed)
		return (0);
	
	i = 0;
	while (trimmed[i] && (trimmed[i] == ' ' || trimmed[i] == '\t'))
		i++;
	
	j = ft_strlen(trimmed) - 1;
	while (j >= i && (trimmed[j] == ' ' || trimmed[j] == '\t'))
		j--;
	trimmed[j + 1] = '\0';
	
	if (i > j)
	{
		free(trimmed);
		return (0);
	}
	
	ft_memmove(trimmed, trimmed + i, j - i + 2);
	
	errno = 0;
	strtol(trimmed, &endptr, 10);
	
	if (errno == ERANGE || *endptr != '\0' || endptr == trimmed)
	{
		free(trimmed);
		return (0);
	}
	
	free(trimmed);
	return (1);
}

static int parse_exit_code(const char *str)
{
	char *trimmed;
	long result;
	int i, j;
	
	if (!str)
		return (0);
	
	trimmed = ft_strdup((char *)str);
	if (!trimmed)
		return (0);
	
	// Trim leading whitespace
	i = 0;
	while (trimmed[i] && (trimmed[i] == ' ' || trimmed[i] == '\t'))
		i++;
	
	j = ft_strlen(trimmed) - 1;
	while (j >= i && (trimmed[j] == ' ' || trimmed[j] == '\t'))
		j--;
	trimmed[j + 1] = '\0';
	
	ft_memmove(trimmed, trimmed + i, j - i + 2);
	
	result = ft_atoi(trimmed);
	free(trimmed);
	
	return ((unsigned char)result);
}

int	builtin_exit(t_shell *shell, char **argv)
{
	int exit_code = 0;
	char *last_exit_status;
	
	if (!shell || !argv)
		return (1);

	if (argv[1] && argv[2])
	{
		ft_putstr_fd("42sh: exit: too many arguments\n", 2);
		return (1);
	}
	
	if (argv[1])
	{
		if (!is_valid_number(argv[1]))
		{
			ft_putstr_fd("42sh: exit: ", 2);
			ft_putstr_fd(argv[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			return (1);
		}
		
		exit_code = parse_exit_code(argv[1]);
	}
	else
	{
		last_exit_status = get_variable(shell->vars, "?");
		if (last_exit_status)
			exit_code = ft_atoi(last_exit_status);
		else
			exit_code = 0;
	}

	ft_putstr_fd("exit\n", 1);
	cleanup_shell(shell);
	exit(exit_code);
}
