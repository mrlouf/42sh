#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"
#include "../../incs/executor.h"

#define FLAG_A (1 << 0)		// 1
#define FLAG_F (1 << 1)		// 2
#define FLAG_P (1 << 2)		// 4
#define FLAG_T (1 << 3)		// 8
#define FLAG_BIG_P (1 << 4) // 16

/*
	!!!LEFT To-Do when command is ALIAS
*/

static int print_type_of(t_shell *shell, const char *cmd, int flags)
{
	char *path;

	// -t print only type
	if (flags & FLAG_T)
	{
		if (is_builtin_command(cmd))
			ft_putendl_fd("builtin", 1);
		else if ((path = find_executable_path(shell, cmd)) != NULL)
		{
			ft_putendl_fd("file", 1);
			free(path);
		}
		else
			; // Nothing to print when not found with -t.
		return (0);
	}

	// -p print only path or nothing
	if (flags & FLAG_P)
	{
		if (is_builtin_command(cmd))
			return (0); // print nothing for builtins
		path = find_executable_path(shell, cmd);
		if (path)
		{
			ft_putendl_fd(path, 1);
			free(path);
		}
		return (0);
	}

	// -a 
	if (flags & FLAG_A)
		printf("FLAG A");

	// -f
	if (flags & FLAG_F)
		printf("FLAG F");

	// -P
	if (flags & FLAG_BIG_P)
		printf("FLAG P");

	// Default output
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

int builtin_type(t_shell *shell, char **argv)
{
	int i = 1;
	int flags = 0;
	char *opt;

	if (!argv[1])
	{
		ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
		shell->last_exit_status = 1;
		return (1);
	}

	while (argv[i] && argv[i][0] == '-')
	{
		opt = argv[i];
		// invalid --
		if (ft_strcmp(opt, "--") == 0)
		{
			i++;
			break;
		}
		// --help and -h
		if (ft_strcmp(opt, "--help") == 0 || ft_strcmp(opt, "-h") == 0)
		{
			ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
			shell->last_exit_status = 0;
			return (0);
		}
		// Invalid "--" or lone "-"
		if (opt[1] == '-' || opt[1] == '\0')
		{
			ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
			shell->last_exit_status = 2;
			return (2);
		}
		// valid
		for (int j = 1; opt[j]; j++)
		{
			if (opt[j] == 'a')
				flags |= FLAG_A;
			else if (opt[j] == 'f')
				flags |= FLAG_F;
			else if (opt[j] == 'p')
			{
				flags &= ~FLAG_T; // Remove previous -t flag if any
				flags |= FLAG_P;  // Set -p flag
			}
			else if (opt[j] == 't')
			{
				flags &= ~FLAG_P; // Remove previous -p flag if any
				flags |= FLAG_T;  // Set -t flag
			}
			else if (opt[j] == 'P')
				flags |= FLAG_BIG_P;
			else
			{
				fprintf(stderr, "type: invalid option -- '%c'\n", opt[j]);
				ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
				shell->last_exit_status = 2;
				return (2);
			}
		}
		i++;
	}
	if (!argv[i])
	{
		ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
		shell->last_exit_status = 1;
		return (1);
	}

	shell->last_exit_status = 0;
	while (argv[i])
	{
		if (print_type_of(shell, argv[i], flags) != 0)
			shell->last_exit_status = 1;
		i++;
	}
	return (shell->last_exit_status);
}

// int	builtin_type(t_shell *shell, char **argv)
// {
// 	int i = 1;
// 	int flags = 0;
// 	int last_flag = 0;
// 	char *opt;

// 	if (!argv[1])
// 	{
// 		ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
// 		shell->last_exit_status = 1;
// 		return (1);
// 	}

// 	while (argv[i] && argv[i][0] == '-')
// 	{
// 		opt = argv[i];
// 		// invalid --
// 		if (ft_strcmp(opt, "--") == 0)
// 		{
// 			i++;
// 			break;
// 		}
// 		// --help and -h
// 		if (ft_strcmp(opt, "--help") == 0 || ft_strcmp(opt, "-h") == 0)
// 		{
// 			ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
// 			shell->last_exit_status = 0;
// 			return (0);
// 		}
// 		// Invalid "--" or lone "-"
// 		if (opt[1] == '-' || opt[1] == '\0')
// 		{
// 			ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
// 			shell->last_exit_status = 2;
// 			return (2);
// 		}
// 		// valid
// 		for (int j = 1; opt[j]; j++)
// 		{
// 			if (opt[j] == 'a')
// 				flags |= FLAG_A;
// 			else if (opt[j] == 'f')
// 				flags |= FLAG_F;
// 			else if (opt[j] == 'p')
// 			{	
// 				flags |= FLAG_P;
// 				last_flag = FLAG_P; // Update last flag
// 			}
// 			else if (opt[j] == 't')
// 			{
// 				flags |= FLAG_T;
// 				last_flag = FLAG_T; // Update last flag
// 			}
// 			else if (opt[j] == 'P')
// 				flags |= FLAG_BIG_P;
// 			else
// 			{
// 				fprintf(stderr, "type: invalid option -- '%c'\n", opt[j]);
// 				ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
// 				shell->last_exit_status = 2;
// 				return (2);
// 			}
// 		}
// 		i++;
// 	}
// 	if (!argv[i])
// 	{
// 		ft_putendl_fd("type: usage: type [-afptP] name [name ...]", 2);
// 		shell->last_exit_status = 1;
// 		return (1);
// 	}

// 	// Checkear ultima flag predominante
// 	int effective_flags = flags;
// 	if (last_flag == FLAG_T)
// 		effective_flags = (flags & ~FLAG_P) | FLAG_T; // Remove FLAG_P, mantain FLAG_T;
// 	else if (last_flag == FLAG_P)
// 		effective_flags = (flags & ~FLAG_T) | FLAG_P; // Remove FLAG_T, mantain FLAG_P;

// 	shell->last_exit_status = 0;
// 	while(argv[i])
// 	{
// 		if (print_type_of(shell, argv[i], effective_flags) != 0) // send effective flags
// 			shell->last_exit_status = 1;
// 		i++;
// 	}
// 	return (shell->last_exit_status);
// }
