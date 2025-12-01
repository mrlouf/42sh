#include "../../incs/42sh.h"
#include "../../incs/main.h"
#include "../../incs/builtins.h"

static t_echo_config	parse_echo_options(char **argv, int *i)
{
	t_echo_config config = {0};
	config.enable_escapes = 0; // POSIX DEFAULT - escapes disabled by default

	if (argv && *argv)
	{
		while (argv[*i] && argv[*i][0] == '-' && argv[*i][1])
		{
			for (int j  = 1; argv[*i][j]; j++)
			{
				switch (argv[*i][j])
				{
					case 'n':
						config.suppress_newline = 1;
						break;
					case 'e':
						config.enable_escapes = 1;
						break;
					case 'E':
						config.enable_escapes = 0;
						break;
					default:
						return (config);
				}
			}
			(*i)++;
		}
	}

	return (config);
}

static int	process_octal_escape(char *str)
{
	int	octal_value = 0;
	int	digits_consumed = 0;
	int	i = 0;

	while (i < 3 && str[i] >= '0' && str[i] <= '7')
	{
		octal_value = octal_value * 8 + (str[i] - '0');
		digits_consumed++;
		i++;
	}

	ft_putchar_fd((char)octal_value, 1);

	return (digits_consumed);
}

static void	process_backlash_escapes(char *str, t_echo_config *config)
{
	for (int i = 0; str[i] && !config->stop_processing; i++)
	{
		if (str[i] == '\\' && str[i + 1])
		{
			i++;
			switch (str[i])
			{
				case 'a':
					ft_putchar_fd('\a', 1); break;
				case 'b':
					ft_putchar_fd('\b', 1); break;
				case 'f':
					ft_putchar_fd('\f', 1); break;
				case 'n':
					ft_putchar_fd('\n', 1); break;
				case 't':
					ft_putchar_fd('\t', 1); break;
				case 'r':
					ft_putchar_fd('\r', 1); break;
				case 'v':
					ft_putchar_fd('\v', 1); break;
				case '\\':
					ft_putchar_fd('\\', 1); break;
				case 'c':
					config->stop_processing = 1;
					config->suppress_newline = 1;
					return;
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					i += process_octal_escape(&str[i]) - 1;
					break;
				default:
					ft_putchar_fd('\\', 1);
					ft_putchar_fd(str[i], 1);
			}
		} else
		{
			ft_putchar_fd(str[i], 1);
		}
	}
}

int	builtin_echo(char **argv)
{
	int				i = 1;
	t_echo_config	config = parse_echo_options(argv, &i);

	for (int j = i; argv[j]; j++)
	{
		if (j > i)
			ft_putchar_fd(' ', 1);

		if (config.enable_escapes)
		{
			process_backlash_escapes(argv[j], &config);
		} else
		{
			ft_putstr_fd(argv[j], 1);
		}
		
		if (config.stop_processing) break;
	}

	if (!config.suppress_newline) ft_putchar_fd('\n', 1);

	return (0);
}
