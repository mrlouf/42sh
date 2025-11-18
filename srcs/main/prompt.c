#include "../incs/42sh.h"

void	create_prompt(t_shell *sh)
{
	char	*user = getenv("USER") ? getenv("USER") : "user";
	char	hostname[HOST_NAME_MAX] = {0};
	gethostname(hostname, HOST_NAME_MAX);
	// char	cwd[PATH_MAX];

	sh->prompt = ft_strdup(user);
	sh->prompt = ft_strjoin(sh->prompt, "@");
	sh->prompt = ft_strjoin(sh->prompt, hostname);
	sh->prompt = ft_strjoin(sh->prompt, ":");
	sh->prompt = ft_strjoin(sh->prompt, "42sh$ ");
	if (!sh->prompt)
	{
		perror("Failed to create prompt");
		exit(EXIT_FAILURE);
	}
}
