#include "../incs/42sh.h"
#include "../incs/executor.h"
#include "../incs/builtins.h"
#include "../incs/data_structures.h"
#include "../incs/env.h"
#include <sys/wait.h>

char *find_executable_path(t_shell *shell, const char *command)
{
	char	*cached_path;
	char	*path_env;
	char	**path_dirs;
	char	*full_path;
	size_t	i;

	if (strchr(command, '/'))
	{
		if (access(command, F_OK | X_OK) == 0)
			return (ft_strdup((char *)command));
		return (NULL);
	}

	cached_path = get_command_path(shell->cmd_cache, command);
	if (cached_path)
	{
		if (access(cached_path, F_OK | X_OK) == 0)
			return (cached_path);
		free(cached_path);
	}

	path_env = get_variable(shell->vars, "PATH");
	if (!path_env)
		return (NULL);

	path_dirs = ft_split(path_env, ':');
	if (!path_dirs)
		return (NULL);

	i = 0;
	while (path_dirs[i])
	{
		full_path = ft_strjoin(path_dirs[i], "/");
		if (full_path)
		{
			char *tmp = full_path;
			full_path = ft_strjoin(tmp, command);
			free(tmp);
		}

		if (full_path && access(full_path, F_OK | X_OK) == 0)
		{
			cache_command_path(shell->cmd_cache, command, full_path);
			ft_array_free((void **)path_dirs);
			return (full_path);
		}
		
		free(full_path);
		i++;
	}

	ft_array_free((void **)path_dirs);
	return (NULL);
}

static char **build_envp_from_var_table(t_var_table *vars)
{
	unsigned int	hash;
	t_var			*tmp;
	size_t			len = 0;
	size_t			i;
	char			**envp;

	for (hash = 0; hash < VAR_HASH_SIZE; hash++)
	{
		tmp = vars->buckets[hash];
		while (tmp)
		{
			if (tmp->exported && tmp->value)
				len++;
			tmp = tmp->next;
		}
	}

	envp = malloc(sizeof(char*) * (len + 1));
	if (!envp)
	{
		ft_putstr_fd("42sh: Error: mem alloc failed\n", 2);
		return (NULL);
	}

	i = 0;
	for (hash = 0; hash < VAR_HASH_SIZE; hash++)
	{
		tmp = vars->buckets[hash];
		while (tmp)
		{
			if (tmp->exported && tmp->value)
			{
				char *name_eq = ft_strjoin(tmp->name, "=");
				if (name_eq)
				{
					envp[i] = ft_strjoin(name_eq, tmp->value);
					free(name_eq);
					i++;
				}
			}
			tmp = tmp->next;
		}
	}
	envp[i] = NULL;

	return (envp);
}

static int	execute_simple_command(t_shell *shell, char **argv)
{
	char	*path;
	char	**envp;
	pid_t	pid;
	int		status;

	// DEBUG / DEV : while there's no parser, I reject space only commands (which will never arrive here after parser is in place)
	char * trimmed_command = ft_strtrim(argv[0], " ");	// DEBUG
	if (!trimmed_command || !trimmed_command[0])		// DEBUG
		return (1);										// DEBUG
	free(trimmed_command);								// DEBUG

	path = find_executable_path(shell, argv[0]);
	if (!path)
	{
		ft_putstr_fd("42sh: ", 2);
		ft_putstr_fd(argv[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		ft_array_free((void **)argv);
		return(127);
	}

	envp = build_envp_from_var_table(shell->vars);

	pid = fork();
	if (pid == 0)
	{
		// CHILD process
		execve(path, argv, envp);
		perror("execve failed");
		exit(127); // TODO: connect this to the final exit gateway when that's done
	}
	else if (pid > 0)
	{
		// PARENT process (waits for child(ren))
		waitpid(pid, &status, 0);

		free(path);
		ft_array_free((void **)argv);
		ft_array_free((void **)envp);
		return (WEXITSTATUS(status));
	}

	// fork FAILED
	perror("fork failed");
	free(path);
	ft_array_free((void **)argv);
	ft_array_free((void **)envp);
	return (1);
}

int	execute_external_command(t_shell *sh, t_ast_node *input_ast)
{
	char **argv = input_ast->argv;
			char **argv_copy = malloc(sizeof(char*) * (ft_array_size((void**)argv) + 1));
			if (!argv_copy)
				return (1);
			
			for (int i = 0; argv[i]; i++)
			{
				argv_copy[i] = ft_strdup(argv[i]);
				if (!argv_copy[i])
				{
					for (int j = 0; j < i; j++)
						free(argv_copy[j]);
					free(argv_copy);
					return (1);
				}
			}
			argv_copy[ft_array_size((void**)argv)] = NULL;

			int exit_code = execute_simple_command(sh, argv_copy);
			return (exit_code);
}

int execute(t_shell *sh, t_ast_node *input_ast)
{
	if (!input_ast || !input_ast->argv || !input_ast->argv[0])
		return (0);

	if (input_ast->redir_count > 0)
	{
		return redirect(sh, input_ast);
	}
	else
	{
		if (is_builtin_command(input_ast->argv[0]))
			return execute_builtin(sh, input_ast->argv);
		else
		{
			return execute_external_command(sh, input_ast);	
		}
	}
}
