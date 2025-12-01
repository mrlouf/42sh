#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/env.h"
#include "../incs/builtins.h"

int	builtin_unset(t_shell *sh, char **argv)
{
	if (!sh || !argv)
		return (1);
	
	if (!argv[1])
		return (0);

	int exit_status = 0;
	
	for (int i = 1; argv[i]; i++)
		if (unset_variable(sh->vars, argv[i]))
		{
			ft_putstr_fd("bash: unset: ", 2);
			ft_putstr_fd(argv[i], 2);
			ft_putstr_fd(": cannot unset: readonly variable\n", 2);
			exit_status = 1;
			continue;
		}
	
	ft_printf("My exit status is:%d\n", exit_status);
	return (0);
}
