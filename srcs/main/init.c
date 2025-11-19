#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/env.h"

void	init_shell(t_shell *shell, char **env)
{
	(void)env;
	shell->prompt = NULL;
	shell->vars = capture_initial_environment(env);
}
