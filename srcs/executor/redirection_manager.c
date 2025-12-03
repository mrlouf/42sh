#include "../incs/42sh.h"
#include "../incs/executor.h"
#include "../incs/builtins.h"
#include "../incs/data_structures.h"
#include "../incs/env.h"

static void restore_redirections(t_saved_fd *saved_fds, int count)
{
	for (int i = 0; i < count; i++)
	{
		dup2(saved_fds[i].saved_fd,saved_fds[i].original_fd);
		close(saved_fds[i].saved_fd);
	}
	free(saved_fds);
}

static void cleanup_saved_fds(t_saved_fd *saved_fds, int i)
{
	for (int j = 0; j < i; j++)
	{
		dup2(saved_fds[j].saved_fd, saved_fds[j].original_fd);
		close(saved_fds[j].saved_fd);
	}
	free(saved_fds);
}

static t_saved_fd *setup_redirections(t_shell *sh, t_redir *redirs, int count)
{
	t_saved_fd *saved_fds = malloc(sizeof(t_saved_fd) * 3); 
	if (!saved_fds)
		return NULL;

	int stdin_saved = 0, stdout_saved = 0;
	int actual_count = 0;
	
	for (int i = 0; i < count; i++)
	{
		if (redirs[i].type == REDIRECT_IN)
		{
			if (!stdin_saved)
			{
				saved_fds[actual_count].original_fd = STDIN_FILENO;
				saved_fds[actual_count].saved_fd = dup(STDIN_FILENO);
				if (saved_fds[actual_count].saved_fd < 0)
				{
					cleanup_saved_fds(saved_fds, actual_count);

					ft_putstr_fd("42sh: ", 2);
					perror("dup failed");

					return (NULL);
				}
				stdin_saved = 1;
				actual_count++;
			}
			
			int fd = open(redirs[i].file, O_RDONLY);
			if (fd < 0)
			{
				cleanup_saved_fds(saved_fds, actual_count);

				ft_putstr_fd("42sh: ", 2);
				perror(redirs[i].file);

				return (NULL);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redirs[i].type == REDIRECT_OUT || redirs[i].type == REDIRECT_APPEND)
		{
			if (!stdout_saved)
			{
				saved_fds[actual_count].original_fd = STDOUT_FILENO;
				saved_fds[actual_count].saved_fd = dup(STDOUT_FILENO);
				if (saved_fds[actual_count].saved_fd < 0)
				{
					cleanup_saved_fds(saved_fds, actual_count);

					ft_putstr_fd("42sh: ", 2);
					perror("dup failed");

					return (NULL);
				}
				stdout_saved = 1;
				actual_count++;
			}
			
			int flags = (redirs[i].type == REDIRECT_APPEND) 
					? (O_WRONLY | O_CREAT | O_APPEND) 
					: (O_WRONLY | O_CREAT | O_TRUNC);
			int fd = open(redirs[i].file, flags, 0644);
			if (fd < 0)
			{
				cleanup_saved_fds(saved_fds, actual_count);

				ft_putstr_fd("42sh: ", 2);
				perror(redirs[i].file);

				return (NULL);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
	}
	
	sh->saved_fd_count = actual_count;
	return saved_fds;
}

int redirect(t_shell *sh, t_ast_node *input_ast)
{
    t_saved_fd *saved_fds = setup_redirections(sh, input_ast->redirs, input_ast->redir_count);
    if (!saved_fds)
        return (1);
    
    int exit_code;
    if (is_builtin_command(input_ast->argv[0]))
        exit_code = execute_builtin(sh, input_ast->argv);
    else
        exit_code = execute_external_command(sh, input_ast);

    restore_redirections(saved_fds, sh->saved_fd_count);

	sh->saved_fd_count = 0;
    
    return (exit_code);
}
