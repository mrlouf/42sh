#include "../incs/42sh.h"
#include "../incs/token.h"

int     check_continuation_line(const char *input)
{
    int len = ft_strlen(input);
    if (len == 0)
        return (0);
    return (input[len - 1] == '\\');
}

char    *complete_input_with_continuation_line(const char *input)
{
    char *additional_input = NULL;
    char *new_input = ft_strdup((char *)input);
    if (!new_input)
        return (NULL);

    while (check_continuation_line(new_input))
    {
        // Remove the trailing backslash
        new_input[ft_strlen(new_input) - 1] = '\0';

        additional_input = readline("> ");
        if (!additional_input)
        {
            printf("Error: unexpected EOF while looking for continuation line\n");
            free(new_input);
            return (NULL);
        }
        char *temp = new_input;
        new_input = ft_strjoin(new_input, "\n");
        free(temp);
        temp = new_input;
        new_input = ft_strjoin(new_input, additional_input);
        free(temp);
        free(additional_input);
    }
    return (new_input);
}
