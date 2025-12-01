#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/builtins.h"

int	builtin_unset(t_shell *sh, char **argv)
{
	(void)sh;
	(void)argv;
	ft_printf("reached:unset\n");
	return (0);
}
