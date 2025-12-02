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
	long result;
	
	if (!str)
		return (0);
	
	// Create a trimmed copy (remove leading/trailing whitespace)
	trimmed = ft_strdup((char *)str);
	if (!trimmed)
		return (0);
	
	// Trim leading whitespace
	i = 0;
	while (trimmed[i] && (trimmed[i] == ' ' || trimmed[i] == '\t'))
		i++;
	
	// Trim trailing whitespace
	j = ft_strlen(trimmed) - 1;
	while (j >= i && (trimmed[j] == ' ' || trimmed[j] == '\t'))
		j--;
	trimmed[j + 1] = '\0';
	
	// Check if empty after trimming
	if (i > j)
	{
		free(trimmed);
		return (0);
	}
	
	// Move trimmed content to start of string
	ft_memmove(trimmed, trimmed + i, j - i + 2);
	
	// Check if it's a valid number
	errno = 0;
	result = strtol(trimmed, &endptr, 10);
	
	// Check for conversion errors
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
	
	// Create a trimmed copy
	trimmed = ft_strdup((char *)str);
	if (!trimmed)
		return (0);
	
	// Trim leading whitespace
	i = 0;
	while (trimmed[i] && (trimmed[i] == ' ' || trimmed[i] == '\t'))
		i++;
	
	// Trim trailing whitespace
	j = ft_strlen(trimmed) - 1;
	while (j >= i && (trimmed[j] == ' ' || trimmed[j] == '\t'))
		j--;
	trimmed[j + 1] = '\0';
	
	// Move trimmed content to start
	ft_memmove(trimmed, trimmed + i, j - i + 2);
	
	// Convert to long and apply modulo 256
	result = ft_atoi(trimmed);
	free(trimmed);
	
	// Ensure exit code is in valid range (0-255)
	return ((unsigned char)result);
}

int	builtin_exit(t_shell *shell, char **argv)
{
	int exit_code = 0;
	char *last_exit_status;
	
	if (!shell || !argv)
		return (1);
	
	// Check for too many arguments
	if (argv[1] && argv[2])
	{
		ft_putstr_fd("42sh: exit: too many arguments\n", 2);
		return (1); // Return error, don't exit shell
	}
	
	if (argv[1])
	{
		// Validate the argument
		if (!is_valid_number(argv[1]))
		{
			ft_putstr_fd("42sh: exit: ", 2);
			ft_putstr_fd(argv[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			return (1); // Return error, don't exit shell
		}
		
		exit_code = parse_exit_code(argv[1]);
	}
	else
	{
		// No argument: use last command exit status (stored in $?)
		last_exit_status = get_variable(shell->vars, "?");
		if (last_exit_status)
			exit_code = ft_atoi(last_exit_status);
		else
			exit_code = 0; // Default if $? not set
	}
	
	// Print exit message and clean up
	ft_putstr_fd("exit\n", 1);
	cleanup_shell(shell);
	exit(exit_code);
}
