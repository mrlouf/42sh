#ifndef EXECUTOR_H
# define EXECUTOR_H

int	execute(t_shell *sh, t_ast_node *input_ast);
char *find_executable_path(t_shell *shell, const char *command);

#endif
