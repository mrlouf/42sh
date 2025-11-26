#include "../incs/42sh.h"
#include "../incs/main.h"
#include "../incs/executor.h"

// DEBUG / DEV function for execution branch
static t_ast_node	*convert_input_to_fake_ast(char *input){
	t_ast_node *fake_input_ast = malloc(sizeof(t_ast_node));
	if (!fake_input_ast)
		return (NULL);
	
	fake_input_ast->argv = malloc(sizeof(char *) * 2);

	fake_input_ast->type = AST_COMMAND;
	fake_input_ast->argv[0] = ft_strdup(input);
	fake_input_ast->argv[1] = NULL;
	fake_input_ast->redirs = NULL;
	fake_input_ast->redir_count = 0;
	fake_input_ast->left = NULL;
	fake_input_ast->right = NULL;
	fake_input_ast->background = 0;

	return (fake_input_ast);
}

// TODO: build a robust, correct free pipeline for ast nodes (if needed)
// This is just a DEBUG / DEV version while building the initial steps of the executor
static void free_ast_node(t_ast_node *node)
{
	if (node && node->argv && node->argv[0])
	{
		free(node->argv[0]);
		free(node->argv);
	}
	free(node);
}

void	shell_mainloop(t_shell *sh)
{
	char *input = NULL;

	while (42)
	{
		create_prompt(sh);
		input = readline(sh->prompt);
		free(sh->prompt);
		if (!input)
			break;
		if (*input)
		{
			add_history(input);
			tokenise_input(sh, input);
			t_ast_node *fake_input_ast = convert_input_to_fake_ast(input); // DEBUG / DEV step for executor branch
			if (!fake_input_ast)
			{
				ft_putstr_fd("Error: Malloc: Fake AST node creation failed\n", 2);
				free(input);
				exit(1);
			}
			sh->last_exit_status = execute(sh, fake_input_ast);

			free(input);
			free_ast_node(fake_input_ast);
		}
		else
			free(input);
	}
}
