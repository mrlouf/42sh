#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/env.h"
#include "../incs/builtins.h"

static int	evaluate_readonly_name(char *name)
{
	if (!name || !*name)
	{
		ft_putstr_fd("readonly: '': not a valid identifier\n", 2);
		return (1);
	}
	
	if (!ft_isalpha(name[0]) && name[0] != '_')
	{
		ft_putstr_fd("readonly: '", 2);
		ft_putstr_fd(name, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (1);
	}
	
	for (int i = 1; name[i]; i++)
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
		{
			ft_putstr_fd("readonly: '", 2);
			ft_putstr_fd(name, 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			return (1);
		}
	}
	
	return (0);
}

static int	parse_readonly_assignment(const char *arg, char **name, char **value)
{
	char *equals = ft_strchr(arg, '=');
	if (!equals)
		return (0);
	
	*name = ft_strndup((char *)arg, equals - arg);
	*value = ft_strdup(equals + 1);
	
	return (1);
}

static void	print_readonly_formatted(t_var **refs, size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		ft_putstr_fd("declare -r ", 1);
		ft_putstr_fd(refs[i]->name, 1);
		if (refs[i]->value)
		{
			ft_putstr_fd("=", 1);
			ft_putstr_fd(refs[i]->value, 1);
		}
		ft_putstr_fd("\n", 1);
	}
}

static int	display_readonly_vars(t_shell *sh)
{
	size_t count;
	t_var **refs = get_sorted_readonly_refs(sh->vars, &count);
	
	if (!refs && count > 0)
	{
		ft_putstr_fd("42sh: Error: Malloc: Bad allocation", 2);
		return (1);
	}
	
	if (refs)
	{
		print_readonly_formatted(refs, count);
		free(refs);
	}
	
	return (0);
}

int	builtin_readonly(t_shell *sh, char **argv)
{
	if (!sh || !sh->vars || !argv)
		return (1);
	
	if (!argv[1])
		return (display_readonly_vars(sh));

	int exit_status = 0;
	
	for (int i = 1; argv[i]; i++)
	{
		char *name, *value;
		
		if (parse_readonly_assignment(argv[i], &name, &value))
		{
			if (evaluate_readonly_name(name))
			{
				free(name);
				free(value);
				exit_status = 1;
				continue;
			}
			
			if (is_variable_readonly(sh->vars, name))
			{
				ft_putstr_fd("readonly: ", 2);
				ft_putstr_fd(name, 2);
				ft_putstr_fd(": readonly variable\n", 2);
				free(name);
				free(value);
				exit_status = 1;
				continue;
			}
			
			if (set_variable(sh->vars, name, value, 0, 1) != 0)
			{
				ft_putstr_fd("readonly: failed to set variable\n", 2);
				free(name);
				free(value);
				exit_status = 1;
				continue;
			}
			
			mark_variable_as_readonly(sh->vars, name);
			free(name);
			free(value);
		}
		else
		{
			if (evaluate_readonly_name(argv[i]))
			{
				exit_status = 1;
				continue;
			}
			
			if (!get_variable(sh->vars, argv[i]))
			{
				ft_putstr_fd("readonly: ", 2);
				ft_putstr_fd(argv[i], 2);
				ft_putstr_fd(": variable not found\n", 2);
				exit_status = 1;
				continue;
			}
			
			mark_variable_as_readonly(sh->vars, argv[i]);
		}
	}
	
	return (exit_status);
}
