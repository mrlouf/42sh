/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nponchon <nponchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 12:11:05 by nponchon          #+#    #+#             */
/*   Updated: 2025/11/17 15:07:26 by nponchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/42sh.h"

int	main(int ac, char **av, char **envp)
{
	ft_printf("%d\n", ac);
	print_array(av);
	print_array(envp);
	return (0);
}
