#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"

int	builtin_type(t_shell *shell,char **argv)
{
	(void)shell;
	ft_putstr_fd(argv[0], 1);
	return (0);
}
