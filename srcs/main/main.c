#include "../../incs/42sh.h"
#include "../../incs/main.h"

int	main(int ac, char **av, char **env)
{
	t_shell	sh;


	(void)ac;
	(void)av;
	init_shell(&sh, env);
	shell_mainloop(&sh);
	cleanup_shell(&sh);

	return (0);
}
