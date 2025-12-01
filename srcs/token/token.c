#include "../incs/42sh.h"
#include "../incs/token.h"

void		tokenise_input(t_shell *sh, char *input)
{
	char **tokens = NULL;

	if (check_continuation_line(input))
		input = complete_input_with_continuation_line(input);
	if (is_missing_quotes(input))
		input = complete_input_with_quotes(input);
	add_history(input);
	write_history(0);

	tokens = split_charset(input, sh->ifs);

	for (int i = 0; tokens[i] != NULL; i++)
	{ printf("\"%s\" ", tokens[i]); }
	printf("\n");
	ft_array_free((void **)tokens);
}
