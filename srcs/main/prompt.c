#include "../incs/42sh.h"

static char	*get_username(void)
{
	char	*user = NULL;
	user = getenv("USER");

	if (user)
		return (ft_strdup(user));

	uid_t	uid = getuid();
	struct	passwd	*pw = getpwuid(uid);

	if (pw && pw->pw_name)
		return (ft_strdup(pw->pw_name));
	else
		return (ft_strdup("user"));
}

static char	*get_hostname(void)
{
	char	*res = NULL;
	char	hostname[HOST_NAME_MAX] = {"hostname"}; // default value
	if (gethostname(hostname, HOST_NAME_MAX) < 0)
	{
		perror("gethostname failed");
		res = ft_strdup("hostname");
	}

	if (ft_strchr(hostname, '.') == NULL) {
		res = ft_strdup(hostname);
	}
	else {
		char	**split = ft_split(hostname, '.');
		if (split && split[0]) {
			res = ft_strdup(split[0]);
			free_array((void **)split);
		}
		else {
			res = ft_strdup(hostname);
		}
	}
	return (res ? res : NULL);
}

static char	*get_cwd(void)
{
	char	*cwd = NULL;
	char	path[PATH_MAX];
	ft_memset (path, 0, sizeof(path));
	if (getcwd(path, sizeof(path)) == NULL) {
		perror("getcwd");
		cwd = ft_strdup("");
	}
	else {
		cwd = ft_strdup(path);
	}
	return (cwd ? cwd : NULL);
}

void	create_prompt(t_shell *sh)
{
	char	*user = get_username();
	char	*hostname = get_hostname();
	char	*cwd = get_cwd();

	if (!user || !hostname || !cwd)
	{
		perror("Failed to get prompt components");
		exit(EXIT_FAILURE);
	}

	// TODO: handle memory allocation failures in the above functions
	// TODO: and use a single malloc with sprintf to create the prompt string
	sh->prompt = ft_strdup("42sh> ");
	sh->prompt = ft_strjoin_free(sh->prompt, user);
	sh->prompt = ft_strjoin_free(sh->prompt, "@");
	sh->prompt = ft_strjoin_free(sh->prompt, hostname);
	sh->prompt = ft_strjoin_free(sh->prompt, ":");
	sh->prompt = ft_strjoin_free(sh->prompt, cwd);
	sh->prompt = ft_strjoin_free(sh->prompt, "$ ");
	
	free(user);
	free(hostname);
	free(cwd);

	if (!sh->prompt)
	{
		perror("Failed to create prompt");
		exit(EXIT_FAILURE);
	}
}
