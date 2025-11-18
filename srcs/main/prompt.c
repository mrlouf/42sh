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
	return (res);
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
	return (cwd);
}

static void	check_home_directory(char **cwd)
{
	char	*home = getenv("HOME");
	if (!home) {
        struct passwd *pw = getpwuid(getuid());
        if (pw) {
            home = pw->pw_dir;
        }
    }
	if (ft_strnstr(*cwd, home, ft_strlen(home)) == *cwd)
	{
		char	*new_cwd = NULL;
		size_t	home_len = ft_strlen(home);
		if ((*cwd)[home_len] == '/' || (*cwd)[home_len] == '\0')
		{
			if ((*cwd)[home_len] == '/')
				new_cwd = ft_strjoin("~", &(*cwd)[home_len]);
			else
				new_cwd = ft_strdup("~");
			free(*cwd);
			*cwd = new_cwd;
		}
	}
}

void	create_prompt(t_shell *sh)
{
	char	prompt[PATH_MAX] = "42sh> ";

	char	*user = get_username();
	char	*hostname = get_hostname();
	char	*cwd = get_cwd();

	if (!user || !hostname || !cwd)
	{
		perror("Failed to get prompt components");
		exit(EXIT_FAILURE);
	}

	check_home_directory(&cwd);

	sprintf(prompt, "42sh> %s@%s:%s$ ", user, hostname, cwd);
	free(user);
	free(hostname);
	free(cwd);

	sh->prompt = ft_strdup(prompt);
	//TODO: add to gc
	if (!sh->prompt)
	{
		perror("Failed to create prompt");
		exit(EXIT_FAILURE);
	}
}
