#include "../incs/42sh.h"
#include "../incs/main.h"

void	shell_mainloop(t_shell *sh)
{
	char *input = NULL;

	while (42)
	{
		create_prompt(sh);
		input = readline(sh->prompt);
		free(sh->prompt);
		if (input)
		{
			if (*input)
				add_history(input);
			free(input);
		}
		else
			continue;
	}
}
