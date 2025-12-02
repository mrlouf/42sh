#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"

int	builtin_cd(t_shell *shell, char **argv)
{
	char	*target_dir;
	char	*current_pwd;
	int		is_dash_option = 0;

	current_pwd = get_variable(shell->vars, "PWD");
	if (!current_pwd)
	{
		current_pwd = getcwd(NULL, 0);
		set_variable(shell->vars, "PWD", current_pwd, 1, 1);
		free(current_pwd);
	}
	
	if (!argv[1])
	{
		target_dir = get_variable(shell->vars, "HOME");
		if (!target_dir)
		{
			ft_putstr_fd("42sh: cd: HOME not set\n", 2);
			return (1);
		}
	}
	else if (ft_strcmp(argv[1], "-") == 0)
	{
		target_dir = get_variable(shell->vars, "OLDPWD");
		if (!target_dir)
		{
			ft_putstr_fd("42sh: cd: OLDPWD not set\n", 2);
			return (1);
		}
		is_dash_option = 1;
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
		if (current_pwd && !get_variable(shell->vars, "PWD"))
			free(current_pwd);
		return (1);
	}

	if (current_pwd)
		set_variable(shell->vars, "OLDPWD", current_pwd, 1, 1);
	
	char *new_pwd = getcwd(NULL, 0);
	if (new_pwd)
	{
		set_variable(shell->vars, "PWD", new_pwd, 1, 1);

		if (is_dash_option)
		{
			ft_putstr_fd(new_pwd, 1);
			ft_putstr_fd("\n", 1);
		}

		free(new_pwd);
	}

	if (current_pwd && !get_variable(shell->vars, "PWD"))
		free(current_pwd);
	
	return (0);
}
