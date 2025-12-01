#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/builtins.h"

int	is_builtin_command(const char *command)
{
	if (!command)
		return (0);
	
	if (!ft_strcmp(command, "exit"))
		return (1);
	if (!ft_strcmp(command, "cd"))
		return (1);
	if (!ft_strcmp(command, "env"))
		return (1);
	if (!ft_strcmp(command, "echo"))
		return (1);
	if (!ft_strcmp(command, "type"))
		return (1);
	if (!ft_strcmp(command, "export"))
		return (1);
	if (!ft_strcmp(command, "readonly"))
		return (1);
	if (!ft_strcmp(command, "unset"))
		return (1);
	
	return (0);
}

int	execute_builtin(t_shell *shell, char **argv)
{
	if (!argv || !argv[0])
		return (1);
	
	if (!ft_strcmp(argv[0], "exit"))
		return (builtin_exit(shell, argv));
	if (!ft_strcmp(argv[0], "cd"))
		return (builtin_cd(shell, argv));
	if (!ft_strcmp(argv[0], "env"))
		return (builtin_env(shell, argv));
	if (!ft_strcmp(argv[0], "echo"))
		return (builtin_echo(argv));
	if (!ft_strcmp(argv[0], "type"))
		return( builtin_type(argv));
	if (!ft_strcmp(argv[0], "export"))
		return( builtin_export(shell, argv));
	if (!ft_strcmp(argv[0], "readonly"))
		return( builtin_readonly(shell, argv));
	if (!ft_strcmp(argv[0], "unset"))
		return( builtin_unset(shell, argv));
	
	return (1); // should never reach here
}
