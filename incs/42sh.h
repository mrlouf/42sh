#ifndef _42SH_H
# define _42SH_H

# include <limits.h>
# include <linux/limits.h>

# include "../libft/libft.h"
# include "data_structures.h"
# include "main.h"

t_var_table	*capture_initial_environment(char **env);
void		print_captured_env(t_var_table *table); // DEBUG
void		print_env_variable(t_var *var); // DEBUG

#endif
