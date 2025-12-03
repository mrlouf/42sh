#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/env.h"
#include "../../incs/builtins.h"

static void	print_set_formatted(t_var **refs, size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		ft_putstr_fd(refs[i]->name, 1);
		ft_putstr_fd("=", 1);
		if (refs[i]->value)
			ft_putstr_fd(refs[i]->value, 1);
		ft_putstr_fd("\n", 1);
	}
}

static int	display_all_variables(t_shell *sh)
{
	size_t count;
	t_var **refs = get_all_sorted_refs(sh->vars, &count);
	
	if (count == 0)
	{
		// No variables to display, this is normal
		return (0);
	}
	
	if (!refs)
	{
		ft_putstr_fd("42sh: Error: Malloc: Bad allocation\n", 2);
		return (1);
	}
	
	print_set_formatted(refs, count);
	free(refs);

	return (0);
}

int	builtin_set(t_shell *sh, char **argv)
{
	// Handle NULL shell
	if (!sh || !sh->vars)
		return (1);
		
	// Handle NULL argv (should work)
	if (!argv)
		return (display_all_variables(sh));
	
	if (argv[1] && !ft_strncmp("-", argv[1], 1))
	{
		ft_putstr_fd("set: invalid option\n", 2);
		return (1);
	}
	else if (argv[1])
	{
		ft_putstr_fd("set: too many arguments\n", 2);
		return (1);
	}

	return (display_all_variables(sh));
}
