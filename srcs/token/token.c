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

	while (input[i]) {
		while (input[i] && ft_strchr(ifs, input[i]))
			i++;
		if (!input[i])
			break;
		else { // Regular word
			start = i;
			while (input[i] && ft_strchr(ifs, input[i]) == NULL) {
				if (input[i] == '\'' || input[i] == '\"') { // Quotes
					skip_quotes(input, &i);
				} else
					i++;
			}
		}
		// Here we would normally extract the token and add it to the list
		// For simplicity, we just print it
		char *token_value = ft_substr(input, start, i - start);
		printf("^%s^ ", token_value); // DEBUG
		free(token_value);
		printf("%d\n", i); // DEBUG
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


}
