#include "../../incs/42sh.h"
#include "../../incs/token.h"

char	*complete_input_with_quotes(char *input)
{
	char *additional_input = NULL;

	do {
		additional_input = readline("> ");
		if (!additional_input)
		{
			printf("Error: unexpected EOF while looking for matching quotes\n");
			return (NULL);
		}
		char *temp = input;
		input = ft_strjoin(input, "\n");
		temp = input;
		input = ft_strjoin(input, additional_input);
		free(temp);
		free(additional_input);
	} while (is_missing_quotes(input));

	return (input);
}

int	is_missing_quotes(const char *input)
{
	int	i = 0;
	int	single_quote_open = 0;
	int	double_quote_open = 0;

	while (input[i])
	{
		if ((input[i] == '\'' || input[i] == '\"')
			&& i > 1 && input[i - 1] == '\\' && input[i - 2] == '\\') {
			if (input[i] == '\'')
				single_quote_open = !single_quote_open;
			else if (input[i] == '\"')
				double_quote_open = !double_quote_open;
		}
 		else if ((input[i] == '\'' || input[i] == '\"') // escape characters -> \" or \'
			&& i > 0 && input[i - 1] == '\\') {
			i++;
			continue;
		}
		else if (input[i] == '\'' && double_quote_open == 0
			&& (i > 0 || input[i - 1] != '\\')) {
			single_quote_open = !single_quote_open;
		}
		else if (input[i] == '\"' && single_quote_open == 0
			&& (i > 0 || input[i - 1] != '\\')) {
			double_quote_open = !double_quote_open;
		}
		i++;
	}
	return (single_quote_open || double_quote_open);
}
