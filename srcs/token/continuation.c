#include "../incs/42sh.h"
#include "../incs/token.h"

char	*join_new_input(char *dst, char *src)
{
	char	*new_input;
	size_t	dst_len = ft_strlen(dst);
	size_t	src_len = ft_strlen(src);

	new_input = (char *)malloc(dst_len + src_len + 1);
	if (!new_input)
		return (NULL);

	ft_memcpy(new_input, dst, dst_len);
	ft_memcpy(new_input + dst_len - 1, "\n", 1);
	ft_memcpy(new_input + dst_len, src, src_len);
	new_input[dst_len + src_len] = '\0';

	free(dst);
	free(src);
	dst = NULL;
	src = NULL;

	return (new_input);
}

int     check_continuation_line(const char *input)
{
    int len = ft_strlen(input);
    if (len == 0)
        return (0);
	else if (input[len - 1] == '\\' && (len == 1 || input[len - 2] != '\\'))
		return (1);
    return (0);
}

char    *complete_input_with_continuation_line(const char *input)
{
    char *additional_input = NULL;
    char *new_input = ft_strdup((char *)input);
    if (!new_input)
        return (NULL);

    while (check_continuation_line(new_input))
    {
        additional_input = readline("> ");
        if (!additional_input)
        {
            printf("Error: unexpected EOF while looking for continuation line\n");
            free(new_input);
            return (NULL);
        }
		new_input = join_new_input(new_input, additional_input);
		if (!new_input)
			return (NULL);
    }
	printf("%s\n", new_input);
    return (new_input);
}
