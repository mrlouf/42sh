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

static int has_flag(char *cmd)
{
	if (cmd[0] == '-')
	{
		if (cmd[1] == '-')
		{
			printf("type: usage: type [-afptP] name [name ...]");
			return (1);
		}
		if (cmd[ft_strlen(cmd) - 1] == 'a')
			return(2);
		if (cmd[ft_strlen(cmd) - 1] == 'f')
			return (3);
		if (cmd[ft_strlen(cmd) - 1] == 'p')
			return (4);
		if (cmd[ft_strlen(cmd) - 1] == 't')
			return (5);
		if (cmd[ft_strlen(cmd) - 1] == 'P')
			return (6);
	}
	return (1);
}
int	builtin_type(t_shell *shell, char **argv)
{
	int i = 1;
	if (!argv[1])
	{
		ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
		return (1);
	}

	int flag = has_flag(argv[1]);
	if (flag == 1)
		return (1);
	for (; argv[i]; i++)
		print_type_of(shell, argv[i]);
	
	return (1);
}
