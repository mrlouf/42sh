#ifndef EXECUTOR_H
# define EXECUTOR_H

int		execute(t_shell *sh, t_ast_node *input_ast);
char	*find_executable_path(t_shell *shell, const char *command);
int		redirect(t_shell *sh, t_ast_node *input_ast);
int		execute_external_command(t_shell *sh, t_ast_node *input_ast);

t_ast_node *fake_parse_input(char *input);
void fake_free_ast_node(t_ast_node *node);
void fake_print_ast_node(t_ast_node *node);  // For debugging

#endif
