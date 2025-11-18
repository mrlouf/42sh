/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   42sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nponchon <nponchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 12:13:41 by nponchon          #+#    #+#             */
/*   Updated: 2025/11/17 17:58:58 by nponchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _42SH_H
# define _42SH_H

# include <limits.h>
# include <linux/limits.h>

/* Readline */
# include <readline/readline.h>
# include <readline/history.h>

# include "../libft/libft.h"
# include "data_structures.h"

/* Functions */
void	init_shell(t_shell *shell, char **env);
void	shell_mainloop(t_shell *sh);
void	create_prompt(t_shell *sh);
void	display_prompt(t_shell *sh);

#endif
