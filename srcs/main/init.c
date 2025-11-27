#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/env.h"
#include "../../incs/data_structures.h"

static char	*init_ifs(void)
{
	char	*ifs = NULL;
	ifs = malloc(sizeof(char) * 127 + 1);
	ft_bzero(ifs, sizeof(char) * 127 + 1);
	if (!ifs)
	{
		ft_putstr_fd("Error: Malloc: IFS initialization failed\n", 2);
		exit(1);
	}
	ifs[0] = ' ';	// load bash's default IFS values
	ifs[1] = '\t';
	ifs[2] = '\n';

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

void	cleanup_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->vars)
		free_var_table(shell->vars);
	if (shell->cmd_cache)
		free_cmd_table(shell->cmd_cache);
	if (shell->prompt)
		free(shell->prompt);
}
