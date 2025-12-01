#include "../incs/42sh.h"

int	count_words(const char *str, const char *charset)
{
	int	i = 0;
	int	in_word = 0;
	int	word_count = 0;

	while (str[i])
	{
		if (ft_strchr(charset, str[i]) != NULL)
			in_word = 0;
		else if (in_word == 0)
		{
			in_word = 1;
			word_count++;
			if (str[i] == '\'' || str[i] == '\"')
			{
				char quote = str[i];
				i++;
				while (str[i] && str[i] != quote)
					i++;
			}
		}
		i++;
	}
	printf("Word count: %d\n", word_count); // DEBUG
	return (word_count);
}

static void	free_tokens(char **tokens, int count)
{
	for (int i = 0; i < count; i++)
		free(tokens[i]);
	free(tokens);
}

static void	get_words(const char *str, const char *charset, char **tokens)
{
	int		i = 0;
	int		j = 0;
	int		start = 0;
	int		in_word = 0;

	while (str[i])
	{

		if (ft_strchr(charset, str[i]) != NULL)
		{
			if (in_word)
			{
				tokens[j] = ft_substr(str, start, i - start);
				if (tokens[j] == NULL) {
					free_tokens(tokens, j);
					return;
				}
				j++;
				in_word = 0;
			}
		}
		else if (in_word == 0)
		{
			in_word = 1;
			start = i;
			if (str[i] == '\'' || str[i] == '\"')
			{
				char quote = str[i];
				i++;
				while (str[i] && str[i] != quote)
					i++;
			}
		}
		i++;
	}
	if (in_word) {
		tokens[j] = ft_substr(str, start, i - start);
		if (tokens[j] == NULL) {
			free_tokens(tokens, j);
			return;
		}
	}
}

char **split_charset(const char *str, const char *charset)
{
	if (str == NULL || charset == NULL)
		return (NULL);

	char	**tokens = NULL;
	int		word_count = 0;

	word_count = count_words(str, charset);
	tokens = malloc((word_count + 1) * sizeof(char *));
	if (tokens == NULL)
		return (NULL);
	tokens[word_count] = NULL;

	get_words(str, charset, tokens);

	return (tokens);
}
