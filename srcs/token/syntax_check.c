#include "../incs/42sh.h"
#include "../incs/token.h"

void	complete_input_with_missing_operand(t_list **tokens, char **input)
{
	// TODO> implement function to complete input with continuation line
	(void)tokens;
	(void)input;
	printf("Error: missing operand\n");
}

int	check_missing_operand(t_list **tokens)
{
	t_list	*current = *tokens;
	while (current)
	{
		// TODO> Differenciate between syntax errors and input to be completed
		if (ft_strcmp(current->content, "|") == 0
			|| ft_strcmp(current->content, "&&") == 0
			|| ft_strcmp(current->content, "||") == 0
			|| ft_strcmp(current->content, ">") == 0
			|| ft_strcmp(current->content, ">>") == 0
			|| ft_strcmp(current->content, "<") == 0
			|| ft_strcmp(current->content, "<<") == 0)
		{
			if (current->next == NULL
				|| ft_strcmp(current->next->content, "|") == 0
				|| ft_strcmp(current->next->content, "&&") == 0
				|| ft_strcmp(current->next->content, "||") == 0
				|| ft_strcmp(current->next->content, ">") == 0
				|| ft_strcmp(current->next->content, ">>") == 0
				|| ft_strcmp(current->next->content, "<") == 0
				|| ft_strcmp(current->next->content, "<<") == 0)
			{
				printf("Syntax error near unexpected token `%s'\n", (char *)current->content);
				return (1);
			}
		}
		current = current->next;
	}
	return (0);
}
