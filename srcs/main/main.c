/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nponchon <nponchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 12:11:05 by nponchon          #+#    #+#             */
/*   Updated: 2025/11/17 17:32:34 by nponchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/42sh.h"

int	main(int ac, char **av, char **env)
{
	t_shell	sh;

	(void)ac;
	(void)av;
	init_shell(&sh, env);
	shell_mainloop(&sh);

	return (0);
}
