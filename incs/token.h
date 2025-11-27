#ifndef TOKEN_H
# define TOKEN_H

void	tokenise_input(t_shell *sh, char *input);
char	**split_charset(const char *str, const char *charset);
int		check_quotes(const char *input);

#endif
