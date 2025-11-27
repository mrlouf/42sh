#include "../../incs/42sh.h"
#include "../../incs/token.h"

int	check_quotes(const char *input)
{
	int	i = 0;
	int	single_quote_open = 0;
	int	double_quote_open = 0;

	while (input[i])
	{
		if (input[i] == '\'' && double_quote_open == 0)
			single_quote_open = !single_quote_open;
		else if (input[i] == '\"' && single_quote_open == 0)
			double_quote_open = !double_quote_open;
		i++;
	}
	return (!single_quote_open && !double_quote_open);
}
