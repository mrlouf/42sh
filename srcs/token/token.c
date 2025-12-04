#include "../incs/42sh.h"
#include "../incs/token.h"

void	skip_quotes(char *input, int *i)
{
	char	quote = input[*i];
	(*i)++;
	while (input[*i] && input[*i] != quote) {
		if ((input[*i] == '\\' && quote == '\"' && input[*i + 1])
			|| (input[*i] == '\\' && quote == '\"' && input[*i + 1] == '\"'))
			(*i) += 2;
		else
			(*i)++;
	}
	if (input[*i] == quote)
		(*i)++;
}

t_list **get_tokens_to_list(char *input, char *ifs)
{
	t_list	**tokens = NULL;
	int		i = 0;
	int		start = 0;

	if (!input)
		return (NULL);

	tokens = (t_list**)malloc(sizeof(t_list **));
	if (!tokens)
		return (NULL);
	*tokens = NULL;

	while (input[i]) {
		while (input[i] && ft_strchr(ifs, input[i]))
			i++;
		if (!input[i])
			break;
		else { // Regular word
			start = i;
			while (input[i] && ft_strchr(ifs, input[i]) == NULL) {
				if ((input[i] == '\'' || input[i] == '\"')
					&& i > 1 && input[i - 1] != '\\') { // Quotes
					skip_quotes(input, &i);
				} else
					i++;
			}
		}
		char *token_value = ft_substr(input, start, i - start);
		if (!token_value)
			return (NULL);
		t_list	*new = ft_lstnew(token_value);
		if (!new)
			return (NULL);
		ft_lstadd_back(tokens, new);
	}
	return (tokens);
}

int	tokenise_input(t_shell *sh, char *input)
{
	t_list	**tokens = NULL;
	int		input_completed = 0;

	while (input_completed == 0) {

		if (check_continuation_line(input)) {
			input = complete_input_with_continuation_line(input);
			continue;
		}
		if (is_missing_quotes(input)) {
			input = complete_input_with_quotes(input);
			continue;
		}
		tokens = get_tokens_to_list(input, sh->ifs);
		if (!tokens) {
			// TODO> handle exit failure from malloc
            fprintf(stderr, "Error: Failed to allocate memory for tokens.\n");
			return (1);
		}
		if (check_missing_operand(tokens)) {
			complete_input_with_missing_operand(tokens, &input);
			ft_lstclear(tokens, free);
			free(tokens);
			continue;
		}
		if (check_missing_redirection(tokens)) {
			ft_lstclear(tokens, free);
			free(tokens);
			return (1);
		}
		input_completed = 1;
	}
	
	// DEBUG: print tokens
	printf("Tokens: ");
	for (t_list *tmp = *tokens; tmp != NULL; tmp = tmp->next) {
		printf("^%s^ ", (char *)tmp->content);
	}
	printf("\n");

	ft_lstclear(tokens, free);
	free(tokens);

	add_history(input);	// Add to history only if the input is valid/complete
	write_history(0);
	return (0);
}
