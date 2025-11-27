#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"

int	is_builtin_command(const char *command)
{
	if (!command)
		return (0);
	
	if (!ft_strcmp(command, "exit"))
		return (1);
	if (!ft_strcmp(command, "cd"))
		return (1);
	if (!ft_strcmp(command, "env"))
		return (1);
	if (!ft_strcmp(command, "echo"))
		return (1);
	if (!ft_strcmp(command, "type"))
		return (1);
	
	return (0);
}

int	execute_builtin(t_shell *shell, char **argv)
{
	if (!argv || !argv[0])
		return (1);
	
	if (!ft_strcmp(argv[0], "exit"))
		return (builtin_exit(shell, argv));
	if (!ft_strcmp(argv[0], "cd"))
		return (builtin_cd(shell, argv));
	if (!ft_strcmp(argv[0], "env"))
		return (builtin_env(shell, argv));
	if (!ft_strcmp(argv[0], "echo"))
		return (builtin_echo(argv));
	if (!ft_strcmp(argv[0], "type"))
		return( builtin_type(argv));
	
	return (1); // should never reach here
}

// EXIT COMMAND
int	builtin_exit(t_shell *shell, char **argv)
{
	int exit_code = 0;
	
	(void)shell; // for the unsued flag. Might lose this after GC implementation.
	
	if (argv[1])
	{
		exit_code = ft_atoi(argv[1]);
	}
	
	ft_putstr_fd("exit\n", 1);
	cleanup_shell(shell);
	exit(exit_code);
}

// CD COMMAND
int	builtin_cd(t_shell *shell, char **argv)
{
	char *target_dir;
	
	if (!argv[1])
	{
		target_dir = get_variable(shell->vars, "HOME");
		if (!target_dir)
		{
			ft_putstr_fd("42sh: cd: HOME not set\n", 2);
			return (1);
		}
	}
	else
	{
		target_dir = argv[1];
	}
	
	if (chdir(target_dir) == -1)
	{
		ft_putstr_fd("42sh: cd: ", 2);
		ft_putstr_fd(target_dir, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		return (1);
	}
	
	char *new_pwd = getcwd(NULL, 0);
	if (new_pwd)
	{
		set_variable(shell->vars, "PWD", new_pwd, 1);
		free(new_pwd);
	}
	
	return (0);
}

// ENV COMMAND
int	builtin_env(t_shell *shell, char **argv)
{
	unsigned int	hash;
	t_var			*tmp;
	
	(void)argv;
	
	for (hash = 0; hash < VAR_HASH_SIZE; hash++)
	{
		tmp = shell->vars->buckets[hash];
		while (tmp)
		{
			if (tmp->exported)
			{
				ft_putstr_fd(tmp->name, 1);
				ft_putstr_fd("=", 1);
				ft_putstr_fd(tmp->value, 1);
				ft_putstr_fd("\n", 1);
			}
			tmp = tmp->next;
		}
	}
	
	return (0);
}

// ECHO COMMAND
static t_echo_config	parse_echo_options(char **argv, int *i)
{
	t_echo_config config = {0};
	config.enable_escapes = 0; // POSIX DEFAULT - escapes disabled by default

	if (argv && *argv)
	{
		while (argv[*i] && argv[*i][0] == '-' && argv[*i][1])
		{
			for (int j  = 1; argv[*i][j]; j++)
			{
				switch (argv[*i][j])
				{
					case 'n':
						config.suppress_newline = 1;
						break;
					case 'e':
						config.enable_escapes = 1;
						break;
					case 'E':
						config.enable_escapes = 0;
						break;
					default:
						// Invalid option - stop processing and treat as argument
						return (config);
				}
			}
			(*i)++;
		}
	}

	return (config);
}

int	process_octal_escape(char *str)
{
	int	octal_value = 0;
	int	digits_consumed = 0;
	int	i = 0;

	while (i < 3 && str[i] >= '0' && str[i] <= '7')
	{
		octal_value = octal_value * 8 + (str[i] - '0');
		digits_consumed++;
		i++;
	}

	ft_putchar_fd((char)octal_value, 1);

	return (digits_consumed);
}

static void	process_backlash_escapes(char *str, t_echo_config *config)
{
	for (int i = 0; str[i] && !config->stop_processing; i++)
	{
		if (str[i] == '\\' && str[i + 1])
		{
			i++;
			switch (str[i])
			{
				case 'a':
					ft_putchar_fd('\a', 1); break;
				case 'b':
					ft_putchar_fd('\b', 1); break;
				case 'f':
					ft_putchar_fd('\f', 1); break;
				case 'n':
					ft_putchar_fd('\n', 1); break;
				case 't':
					ft_putchar_fd('\t', 1); break;
				case 'r':
					ft_putchar_fd('\r', 1); break;
				case 'v':
					ft_putchar_fd('\v', 1); break;
				case '\\':
					ft_putchar_fd('\\', 1); break;
				case 'c':
					config->stop_processing = 1;
					config->suppress_newline = 1;
					return;
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					i += process_octal_escape(&str[i]) - 1;
					break;
				default:
					ft_putchar_fd('\\', 1);
					ft_putchar_fd(str[i], 1);
			}
		} else
		{
			ft_putchar_fd(str[i], 1);
		}
	}
}

int	builtin_echo(char **argv)
{
	int				i = 1;
	t_echo_config	config = parse_echo_options(argv, &i);

	for (int j = i; argv[j]; j++)
	{
		if (j > i)
			ft_putchar_fd(' ', 1);

		if (config.enable_escapes)
		{
			process_backlash_escapes(argv[j], &config);
		} else
		{
			ft_putstr_fd(argv[j], 1);
		}
		
		if (config.stop_processing) break;
	}

	if (!config.suppress_newline) ft_putchar_fd('\n', 1);

	return (0);
}

// TYPE COMMAND
int	builtin_type(char **argv)
{
	ft_putstr_fd(argv[0], 1);
	return (0);
}

