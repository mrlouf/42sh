#include "../incs/42sh.h"
#include "../incs/data_structures.h"
#include "../incs/main.h"

int	main(int ac, char **av, char **env)
{
	t_shell	*sh = malloc(sizeof(t_shell));
	if (!sh)
	{
		ft_putstr_fd("42sh: Error: mem alloc failed\n", 2);
		return (1);
	}

	(void)ac;
	(void)av;
	init_shell(sh, env);
	shell_mainloop(sh);
	cleanup_shell(sh);

	return (0);
}
