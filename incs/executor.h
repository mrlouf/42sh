#ifndef EXECUTOR_H
# define EXECUTOR_H

// Function prototypes
int	execute(t_shell *sh, t_ast_node *input_ast);

// DEBUG / DEV fake parser functions (remove when real parser is ready)
t_ast_node *fake_parse_input(char *input);
void fake_free_ast_node(t_ast_node *node);
void fake_print_ast_node(t_ast_node *node);  // For debugging

#endif
