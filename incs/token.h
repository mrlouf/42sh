#ifndef TOKEN_H
# define TOKEN_H

# include "main.h"

void	tokenise_input(t_shell *sh, char *input);
char	**split_charset(const char *str, const char *charset);
int		check_quotes(const char *input);
char	*complete_input_with_quotes(char *input);

#endif
