#ifndef _42SH_H
# define _42SH_H

# include <limits.h>
# include <linux/limits.h>

/* Readline */
# include <readline/readline.h>
# include <readline/history.h>

# include "../libft/libft.h"
# include "data_structures.h"

/* Functions */
void		init_shell(t_shell *shell, char **env);
void		shell_mainloop(t_shell *sh);
void		create_prompt(t_shell *sh);
void		display_prompt(t_shell *sh);

t_var_table	*capture_initial_environment(char **env);
void		print_captured_env(t_var_table *table); // DEBUG
void		print_env_variable(t_var *var); // DEBUG

#endif
