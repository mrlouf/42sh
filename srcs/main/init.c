#include "../incs/42sh.h"

void	init_shell(t_shell *shell, char **env)
{
	(void)env;
	shell->prompt = NULL;
	shell->vars = capture_initial_environment(env);
	//DEBUG
	print_captured_env(shell->vars);
}
