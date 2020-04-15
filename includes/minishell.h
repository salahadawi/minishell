/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadawi <sadawi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/10 12:20:24 by sadawi            #+#    #+#             */
/*   Updated: 2020/04/15 17:14:58 by sadawi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/includes/libft.h"
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <dirent.h>
# include <signal.h>
# include <fcntl.h>

typedef struct	s_env
{
	char		**envp;
}				t_env;

#endif
