#include "../incs/42sh.h"

int	count_words(const char *str, const char *charset)
{
	int		i = 0;
	int		in_word = 0;
	int		word_count = 0;
	char	quote_open = 0;

	while (str[i])
	{
		if (str[i] == '\'' && !quote_open)
			quote_open = !quote_open;
		else if (str[i] == '"' && !quote_open)
			quote_open = !quote_open;
		else if (ft_strchr(charset, str[i]) && !quote_open) 
			in_word = 0;

		if (in_word == 0)
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
    int		i = 0, j = 0, start = -1;
    int		in_single = 0, in_double = 0;

    while (str[i])
    {
        if (str[i] == '\'' && !in_double)
            in_single = !in_single;
        else if (str[i] == '"' && !in_single)
            in_double = !in_double;

        if (!in_single && !in_double && ft_strchr(charset, str[i]))
        {
            if (start != -1)
            {
                tokens[j] = ft_substr(str, start, i - start);
                if (tokens[j] == NULL) {
                    free_tokens(tokens, j);
                    return;
                }
                j++;
                start = -1;
            }
        }
        else
        {
            if (start == -1)
                start = i;
        }
        i++;
    }
    if (start != -1) {
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
