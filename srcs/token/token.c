#include "../../incs/42sh.h"
#include "../../incs/token.h"

void	tokenise_input(t_shell *sh, char *input)
{
	char **tokens = NULL;

	printf("Missing closing quotes: %s\n", check_quotes(input) ? "No" : "Yes");
/* 	if (!check_quotes(input))
	{
		// get new prompt for closing quotes
		// wait for completing input and join it to original one

	} */
	tokens = split_charset(input, sh->ifs);

	for (int i = 0; tokens[i] != NULL; i++)
	{ printf("\"%s\" ", tokens[i]); }
	printf("\n");
}
