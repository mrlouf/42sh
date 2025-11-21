#include "../incs/42sh.h"
#include "../incs/main.h"

char    **get_tokens(char *input) {
    // Placeholder for tokenization logic
    // This function should split the input string into tokens
    return ft_split(input, ' '); // Simple split by space for demonstration
}

void    parse_input(t_shell *sh, char *input) {
    // Placeholder for the actual parsing logic

    (void)sh;
    char    **tokens = get_tokens(input);

    if (!tokens) {
        perror("Tokenization failed");
        return;
    }
    ft_printf("Parsed tokens:\n");
    for (int i = 0; tokens[i]; i++) {
        ft_printf("Token %d: %s\n", i, tokens[i]);
    }
    // Free tokens after use
    for (int i = 0; tokens[i]; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
