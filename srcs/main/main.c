#include "../incs/42sh.h"
#include "../incs/main.h"

int	main(int ac, char **av, char **env)
{
	t_shell	sh;

	if (!isatty(STDIN_FILENO)) {
		printf("42sh$ ");
		fflush(stdout);
		return (0);
	}

	(void)ac;
	(void)av;
	init_shell(&sh, env);
	shell_mainloop(&sh);

	return (0);
}
