#ifndef MAIN_H
# define MAIN_H

/* Readline */
# include <readline/readline.h>
# include <readline/history.h>

void		init_shell(t_shell *shell, char **env);
void		cleanup_shell(t_shell *shell);
void		shell_mainloop(t_shell *sh);
void		create_prompt(t_shell *sh);
void		display_prompt(t_shell *sh);

#endif
