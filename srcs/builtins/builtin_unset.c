#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/env.h"
#include "../../incs/builtins.h"

static int	evaluate_unset_name(char *name)
{
	if (!name || !*name)
	{
		ft_putstr_fd("unset: '': not a valid identifier\n", 2);
		return (1);
	}
	
	if (!ft_isalpha(name[0]) && name[0] != '_')
	{
		ft_putstr_fd("unset: '", 2);
		ft_putstr_fd(name, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (1);
	}
	
	for (int i = 1; name[i]; i++)
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
		{
			ft_putstr_fd("unset: '", 2);
			ft_putstr_fd(name, 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			return (1);
		}
	}
	
	return (0);
}

int	builtin_unset(t_shell *sh, char **argv)
{
	if (!sh || !sh->vars || !argv)
		return (1);
	
	if (!argv[1])
		return (0);

	int exit_status = 0;
	
	for (int i = 1; argv[i]; i++)
	{
		if (evaluate_unset_name(argv[i]))
		{
			exit_status = 1;
			continue;
		}
		
		if (is_variable_readonly(sh->vars, argv[i]))
		{
			ft_putstr_fd("unset: ", 2);
			ft_putstr_fd(argv[i], 2);
			ft_putstr_fd(": cannot unset: readonly variable\n", 2);
			exit_status = 1;
			continue;
		}
		
		unset_variable(sh->vars, argv[i]);
	}
	
	return (exit_status);
}
