#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/env.h"
#include "../incs/data_structures.h"

static char *init_ifs()
{
	char	*ifs = malloc(sizeof(char) * 127);
	
	if (!ifs)
	{
		ft_putstr_fd("Error: Failed to allocate memory for IFS\n", 2);
		exit(1);
	}

	ft_bzero(ifs, 127);
	ifs[' '] = 1;
	ifs['\t'] = 1;
	ifs['\n'] = 1;
	return (ifs);
}

void	init_shell(t_shell *shell, char **env)
{
	(void)env;
	shell->prompt = NULL;
	shell->ifs = init_ifs();
	shell->vars = capture_initial_environment(env);
	shell->cmd_cache = init_cmd_table();
	if (!shell->cmd_cache)
	{
		ft_putstr_fd("Error: Failed to initialize command cache\n", 2);
		exit(1);
	}
}
