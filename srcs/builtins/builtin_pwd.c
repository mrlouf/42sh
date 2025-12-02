#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/env.h"
#include "../../incs/builtins.h"

static int validate_logical_pwd(const char *logical_pwd)
{
	if (!logical_pwd || !*logical_pwd)
		return (0);
	
	if (logical_pwd[0] != '/')
		return (0);

	char *real_pwd = getcwd(NULL, 0);
	if (!real_pwd)
		return (0);
	
	struct stat logical_stat, real_stat;
	int result = 0;
	
	if (stat(logical_pwd, &logical_stat) == 0 && 
		stat(real_pwd, &real_stat) == 0)
	{
		result = (logical_stat.st_dev == real_stat.st_dev && 
				logical_stat.st_ino == real_stat.st_ino);
	}
	
	free(real_pwd);
	return (result);
}

static int parse_pwd_args(char **argv, t_pwd_config *config)
{
	int exit_status = 0;
	config->logical = 0;
	config->physical = 0;

	for (int i = 1; argv[i]; i++)
	{
		if (ft_strcmp(argv[i], "--") == 0)
			break;
		
		if (argv[i][0] == '-' && argv[i][1] != '\0')
		{
			for (int j = 1; argv[i][j]; j++)
			{
				if (argv[i][j] == 'L')
				{
					config->logical = 1;
					config->physical = 0;
				}
				else if (argv[i][j] == 'P')
				{
					config->physical = 1;
					config->logical = 0;
				}
				else
				{
					ft_putstr_fd("42sh: pwd: -", 2);
					ft_putchar_fd(argv[i][j], 2);
					ft_putstr_fd(": invalid option\n", 2);
					ft_putstr_fd("pwd: usage: pwd [-LP]\n", 2);
					exit_status = 2;
				}
			}
		}
	}
	
	return exit_status;
}

int	builtin_pwd(t_shell *shell, char **argv)
{
	t_pwd_config config = {0, 0};
	int exit_status = 0;

	if (!shell || !shell->vars)
		return (1);

	if (argv && argv[1])
	{
		exit_status = parse_pwd_args(argv, &config);
		
		if (!config.logical && !config.physical)
			config.logical = 1;
	}
	else
	{
		config.logical = 1;  // Default POSIX behavior
	}

	char *pwd_to_print = NULL;

	if (config.physical)
	{
		pwd_to_print = getcwd(NULL, 0);
	}
	else
	{
		char *logical_pwd = get_variable(shell->vars, "PWD");
		if (logical_pwd && validate_logical_pwd(logical_pwd))
			pwd_to_print = ft_strdup(logical_pwd);
		else
			pwd_to_print = getcwd(NULL, 0);
	}

	if (!pwd_to_print)
	{
		if (errno == EACCES)
			ft_putstr_fd("pwd: permission denied\n", 2);
		else if (errno == ENOENT)
			ft_putstr_fd("pwd: current directory has been removed\n", 2);
		else if (errno == ENAMETOOLONG)
			ft_putstr_fd("pwd: path name too long\n", 2);
		else
			ft_putstr_fd("pwd: cannot get current directory\n", 2);
		return (1);
	}
	
	ft_putstr_fd(pwd_to_print, 1);
	ft_putstr_fd("\n", 1);
	
	free(pwd_to_print);
	return (exit_status);
}
