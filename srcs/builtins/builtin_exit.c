#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"

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
