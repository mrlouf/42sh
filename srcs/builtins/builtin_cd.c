#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"

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
		set_variable(shell->vars, "PWD", new_pwd, 1, 1);
		free(new_pwd);
	}
	
	return (0);
}
