#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"
#include "../../incs/executor.h"

static int print_type_of(t_shell *shell, const char *cmd)
{
	char *path;
	if (is_builtin_command(cmd))
	{
		printf("%s is a shell builtin\n", cmd);
		return (0);
	}
	path = find_executable_path(shell, cmd);
	if (path)
	{
		printf("%s is %s\n", cmd, path);
		free(path);
		return (0);
	}

	fprintf(stderr, "type: %s: not found\n", cmd);
	shell->last_exit_status = 1;
	return (1);
} 

int	builtin_type(t_shell *shell,char **argv)
{
	int i = 1;
	if (!argv[1])
	{
		ft_putendl_fd("type: usage: type name [name ...]", 2);
		return (1);
	}
	for (; argv[i]; i++)
		print_type_of(shell, argv[i]);
	
	return (1);
}
