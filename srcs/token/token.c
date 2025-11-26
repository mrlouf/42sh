void	tokenise_input(t_shell *sh, char *input)
{
	char **tokens = NULL;

	tokens = split_input(input, sh->ifs);

	for (int i = 0; tokens[i] != NULL; i++)
	{ printf("\"%s \"", tokens[i]); }
	printf("\n");
}
