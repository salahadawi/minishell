/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadawi <sadawi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/10 12:20:24 by sadawi            #+#    #+#             */
/*   Updated: 2020/04/16 21:54:34 by sadawi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/includes/libft.h"
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <dirent.h>
# include <linux/limits.h>
# include <signal.h>
# include <fcntl.h>

# define BOLDBLUE "\033[1m\033[36m"
# define RED "\033[1m\033[31m"
# define RESET "\033[0m"

struct s_env;

typedef int			t_builtin_func (struct s_env *env, char **args);

typedef struct		s_env
{
	char			**envp;
	char			**builtin_names;
	t_builtin_func	**builtin_funcs;
}					t_env;

#endif
