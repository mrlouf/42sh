#include "../incs/42sh.h"
#include "../incs/executor.h"
#include <sys/wait.h>

static char *find_executable_path(const char *command, t_var_table *vars)
{
	char	*path_env;
	char	**path_dirs;
	char	*full_path;
	size_t	i;

	if (strchr(command, '/'))
		return (ft_strdup((char *)command));

	path_env = get_variable(vars, "PATH");
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
			free_array((void **)path_dirs);
			return (full_path);
		}
		
		free(full_path);
		i++;
	}

	free_array((void **)path_dirs);
	return (NULL);
}

static char **build_envp_from_var_table(t_var_table *vars)
{
	unsigned int	hash;
	t_var			*tmp;
	size_t			len = 0;
	size_t			i;
	char			**envp;

	// Count exported variables
	for (hash = 0; hash < VAR_HASH_SIZE; hash++)
	{
		tmp = vars->buckets[hash];
		while (tmp)
		{
			if (tmp->exported)
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
			if (tmp->exported)
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

static int	execute_simple_command(t_shell *shell, char *command)
{
	char	*path;
	char	**argv;
	char	**envp;
	pid_t	pid;
	int		status;

	path = find_executable_path(command, shell->vars);
	if (!path)
	{
		ft_putstr_fd("42sh: ", 2);
		ft_putstr_fd(command, 2);
		ft_putstr_fd(": command not found\n", 2);
		return(127);
	}

	argv = malloc(sizeof(char*) * 2);
	if (!argv)
	{
		ft_putstr_fd("42sh: Error: mem alloc failed\n", 2);
		free(path);
		return(1);
	}
	argv[0] = ft_strdup(command);
	argv[1] = NULL;

	envp = build_envp_from_var_table(shell->vars);

	pid = fork();
	if (pid == 0)
	{
		// CHILD process
		execve(path, argv, envp);
		perror("execve failed");
		exit(127);
	}
	else if (pid > 0)
	{
		// PARENT process (waits for child(ren))
		waitpid(pid, &status, 0);

		free(path);
		free_array((void **)argv);
		free_array((void **)envp);
		return (WEXITSTATUS(status));
	}

	// fork FAILED
	perror("fork failed");
	free(path);
	free_array((void **)argv);
	free_array((void **)envp);
	return (1);
}

int	execute(t_shell *sh, t_ast_node *input_ast)
{
	if (input_ast->argv && input_ast->argv[0])
	{
		return (execute_simple_command(sh, input_ast->argv[0]));
	}

	return (0);
}
