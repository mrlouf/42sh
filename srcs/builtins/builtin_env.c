#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"

int	builtin_env(t_shell *shell, char **argv)
{
	unsigned int	hash;
	t_var			*tmp;
	
	// POSIX: env should reject any options
	if (argv && argv[1])
	{
		if (argv[1][0] == '-' && argv[1][1] != '\0')
		{
			ft_putstr_fd("env: invalid option -- '", 2);
			ft_putchar_fd(argv[1][1], 2);
			ft_putstr_fd("'\n", 2);
			return (125);
		}

		ft_putstr_fd("env: ", 2);
		ft_putstr_fd(argv[1], 2);
		ft_putstr_fd(": command not found\n", 2);
		return (127);
	}
	
	if (!shell || !shell->vars)
		return (1);
	
	for (hash = 0; hash < VAR_HASH_SIZE; hash++)
	{
		tmp = shell->vars->buckets[hash];
		while (tmp)
		{
			if (tmp->exported)
			{
				ft_putstr_fd(tmp->name, 1);
				ft_putstr_fd("=", 1);
				
				if (tmp->value)
					ft_putstr_fd(tmp->value, 1);
				ft_putstr_fd("\n", 1);
			}
			tmp = tmp->next;
		}
	}
	
	return (0);
}
