#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"

int	builtin_type(char **argv)
{
	ft_putstr_fd(argv[0], 1);
	return (0);
}
