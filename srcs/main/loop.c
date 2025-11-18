/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nponchon <nponchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 17:10:35 by nponchon          #+#    #+#             */
/*   Updated: 2025/11/17 17:34:43 by nponchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/42sh.h"

void	shell_mainloop(t_shell *sh)
{
	char *input = NULL;

	while (42)
	{
		create_prompt(sh);
		input = readline(sh->prompt);
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
