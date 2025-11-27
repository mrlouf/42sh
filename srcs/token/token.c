#include "../../incs/42sh.h"
#include "../incs/main.h"
#include "../../incs/token.h"

void	tokenise_input(t_shell *sh, char *input)
{
	char **tokens = NULL;

	printf("Missing closing quotes: %s\n", check_quotes(input) ? "No" : "Yes");
	if (!check_quotes(input))
	{
		// get new prompt for closing quotes
		char *additional_input = NULL;
		while (!check_quotes(input))
		{
			additional_input = readline("> ");
			if (!additional_input)
			{
				printf("Error: unexpected EOF while looking for matching quotes\n");
				return ;
			}
			char *temp = input;
			input = ft_strjoin(input, "\n");
			temp = input;
			input = ft_strjoin(input, additional_input);
			free(temp);
			free(additional_input);
		}
	}
	add_history(input);
	write_history(0);
	tokens = split_charset(input, sh->ifs);

	for (int i = 0; tokens[i] != NULL; i++)
	{ printf("\"%s\" ", tokens[i]); }
	printf("\n");
}
