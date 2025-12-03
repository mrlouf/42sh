#ifndef BUILTINS_H
# define BUILTINS_H

int		is_builtin_command(const char *command);
int		execute_builtin(t_shell *shell, char **argv);

// Individual built-in functions
int		builtin_exit(t_shell *shell, char **argv);
int		builtin_cd(t_shell *shell, char **argv);
int		builtin_env(t_shell *shell, char **argv);
int		builtin_echo(char **argv);
int		builtin_type(t_shell *shell, char **argv);
int		builtin_export(t_shell *shell, char **argv);

#endif
