/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_charset.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nponchon <nponchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:35:03 by nponchon          #+#    #+#             */
/*   Updated: 2025/11/26 14:45:29 by nponchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/libft.h"

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
		}
		i++;
	}
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
				if (tokens[j] == NULL)
					return free_tokens(tokens, j);
				j++;
				in_word = 0;
			}
		}
		else if (in_word == 0)
		{
			in_word = 1;
			start = i;
		}
		i++;
	}
	if (in_word) {
		tokens[j] = ft_substr(str, start, i - start);
		if (tokens[j] == NULL)
			return free_tokens(tokens, j);
	}
}

char **ft_split_charset(const char *str, const char *charset)
{
	char	**tokens = NULL;
	int		word_count = 0;

	word_count = count_words(str, charset);
	printf("Word count: %d\n", word_count);
	tokens = malloc((word_count + 1) * sizeof(char *));
	if (tokens == NULL)
		return (NULL);
	tokens[word_count] = NULL;

	get_words(str, charset, tokens);

	return (tokens);
}
