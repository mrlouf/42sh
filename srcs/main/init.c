#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/env.h"
#include "../incs/data_structures.h"

void	init_shell(t_shell *shell, char **env)
{
	(void)env;
	shell->prompt = NULL;
	shell->vars = capture_initial_environment(env);
	shell->cmd_cache = init_cmd_table();
	if (!shell->cmd_cache)
	{
		ft_putstr_fd("Error: Failed to initialize command cache\n", 2);
		exit(1);
	}
}
