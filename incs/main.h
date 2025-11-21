#ifndef MAIN_H
# define MAIN_H

/* Readline */
# include <readline/readline.h>
# include <readline/history.h>

void	init_shell(t_shell *shell, char **env);
void	shell_mainloop(t_shell *sh);
void	create_prompt(t_shell *sh);
void	display_prompt(t_shell *sh);

void	parse_input(t_shell *sh, char *input);

#endif
