#include "../incs/42sh.h"
#include "../incs/builtins.h"

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
	
	return (1); // should never reach here
}

int	builtin_exit(t_shell *shell, char **argv)
{
	int exit_code = 0;
	
	(void)shell; // for the unsued flag. Might lose this after GC implementation.
	
	if (argv[1])
	{
		exit_code = ft_atoi(argv[1]);
	}
	
	ft_putstr_fd("exit\n", 1);
	exit(exit_code);
}

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
