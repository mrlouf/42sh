#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"

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
