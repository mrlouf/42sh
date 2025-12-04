#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"
#include "../../incs/env.h"

static void	print_export_formatted(t_shell *sh, t_var **refs, size_t count)
{
	for (size_t i = 0; i < count; i++)
		{
			ft_putstr_fd("declare -", 1);
			if (is_variable_readonly(sh->vars, refs[i]->name))
				ft_putstr_fd("r", 1);
			ft_putstr_fd("x ", 1);
			ft_putstr_fd(refs[i]->name, 1);
			if (refs[i]->value)
			{
				ft_putstr_fd("=\"", 1);
				ft_putstr_fd(refs[i]->value, 1);
				ft_putstr_fd("\"", 1);
			}
			ft_putstr_fd("\n", 1);
		}
}

static int	evaluate_export_name(char *name)
{
	if (!name || !*name)
	{
		ft_putstr_fd("export: '': not a valid identifier\n", 2);
		return (1);
	}
	
	if (!ft_isalpha(name[0]) && name[0] != '_')
	{
		ft_putstr_fd("export: '", 2);
		ft_putstr_fd(name, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (1);
	}
	
	for (int i = 1; name[i]; i++)
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
		{
			ft_putstr_fd("export: '", 2);
			ft_putstr_fd(name, 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			return (1);
		}
	}
	
	return (0);
}

static int parse_assignment(const char *arg, char **name, char **value)
{
	char *equals = ft_strchr(arg, '=');
	if (!equals)
		return (0);
	
	*name = ft_strndup((char *)arg, equals - arg);
	*value = ft_strdup(equals + 1);
	
	return (1);
}

static int	display_exported_vars(t_shell *sh)
{
	size_t count;
	t_var **refs = get_sorted_exported_refs(sh->vars, &count);
	
	if (!refs)
	{
		ft_putstr_fd("42sh: Error: Malloc: Bad allocation", 2);
		return (1);
	}
	
	print_export_formatted(sh, refs, count);
	free(refs);

	return (0);
}

int	builtin_export(t_shell *sh, char **argv)
{
	if (!sh || !sh->vars || !argv)
		return (1);
	
	if (!argv[1])
		return (display_exported_vars(sh));

	int exit_status = 0;
	
	for (int i = 1; argv[i]; i++)
	{
		char *name, *value;
		
		if (parse_assignment(argv[i], &name, &value))
		{
			if (evaluate_export_name(name))
			{
				free(name);
				free(value);
				exit_status = 1;
				continue;
			}
			
			if (set_variable(sh->vars, name, value, 1, 1) != 0)
			{
				if (!is_variable_readonly(sh->vars, name))
					ft_putstr_fd("export: failed to set variable\n", 2);
				free(name);
				free(value);
				exit_status = 1;
				continue;
			}
			
			free(name);
			free(value);
		}
		else
		{
			if (evaluate_export_name(argv[i]))
			{
				exit_status = 1;
				continue;
			}
			
			if (set_variable(sh->vars, argv[i], NULL, 1, 0) != 0)
			{
				if (!is_variable_readonly(sh->vars, argv[i]))
					ft_putstr_fd("export: failed to set variable\n", 2);
				exit_status = 1;
				continue;
			}
		}
	}
	
	return (exit_status);
}
