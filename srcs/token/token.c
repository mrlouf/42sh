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

void	tokenise_input(t_shell *sh, char *input)
{
	t_list **tokens = NULL;

	if (check_continuation_line(input))
		input = complete_input_with_continuation_line(input);
	if (is_missing_quotes(input))
		input = complete_input_with_quotes(input);
	add_history(input);
	write_history(0);

	tokens = get_tokens_to_list(input, sh->ifs);
	
	// DEBUG: print tokens
	printf("Tokens: ");
	for (t_list *tmp = *tokens; tmp != NULL; tmp = tmp->next) {
		printf("^%s^ ", (char *)tmp->content);
	}
	printf("\n");

	ft_lstclear(tokens, free);
	free(tokens);
}
