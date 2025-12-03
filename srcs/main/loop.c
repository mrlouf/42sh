#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/executor.h"

void	shell_mainloop(t_shell *sh)
{
	char *input = NULL;

	while (42)
	{
		create_prompt(sh);
		input = readline(sh->prompt);
		free(sh->prompt);
		sh->prompt = NULL;
		if (!input)
			break;
		if (*input)
		{
			add_history(input);
			
			// DEBUG / DEV: Use fake parser for executor development
			t_ast_node *ast = fake_parse_input(input);
			if (!ast)
			{
				ft_putstr_fd("Error: Failed to parse input\n", 2);
				free(input);
				continue;
			}
			
			// DEBUG: Print AST for debugging (comment out when not needed)
			// fake_print_ast_node(ast);
			
			sh->last_exit_status = execute(sh, ast);
			
			fake_free_ast_node(ast);
		}
		free(input);
	}
}
