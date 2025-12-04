#include "../../incs/42sh.h"
#include "../../incs/token.h"

void	complete_input_with_missing_operand(t_list **tokens, char **input)
{
	// TODO> implement function to complete input with continuation line
	(void)tokens;
	(void)input;
	printf("Error: missing operand\n");
}

int	check_missing_operand(t_list **tokens)
{
	(void)tokens;
	return (0);
}

int	is_redirection(const char *token)
{
	return (ft_strcmp(token, ">") == 0
		|| ft_strcmp(token, ">>") == 0
		|| ft_strcmp(token, "<") == 0
		|| ft_strcmp(token, "<<") == 0
		|| ft_strcmp(token, ">&") == 0
		|| ft_strcmp(token, "<&") == 0);
}

int	check_missing_redirection(t_list **tokens)
{
	t_list	*current = *tokens;
	while (current)
	{
		// TODO> Differenciate between syntax errors and input to be completed
		// TODO> Redirections yield an error if no operand is found
		// TODO> Pipes and logical operators yield a continuation line
		if (is_redirection((char *)current->content)
			&& (current->next == NULL || is_redirection((char *)current->next->content)))
		{
			{
				fprintf(stderr, "Syntax error near unexpected token `%s'\n", (char *)current->content);
				return (1);
			}
		}
		current = current->next;
	}
	return (0);
}
