/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadawi <sadawi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/15 12:28:40 by sadawi            #+#    #+#             */
/*   Updated: 2020/04/15 17:21:07 by sadawi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

void	print_error(char *message)
{
	ft_fprintf(2, "%s\n", message);
	free(message);
}

int		count_env_amount(char *envp[])
{
	int i;

	i = 0;
	while (envp[i])
		i++;
	return (i);
}

char	*ft_strsubchar(char const *s, size_t start, char c)
{
	char	*str;
	size_t	len;
	size_t	i;

	len = start;
	while (s[len] && s[len] != c)
		len++;
	str = (char*)ft_memalloc(len + 1);
	i = 0;
	while (len)
	{
		str[i++] = s[start++];
		len--;
	}
	str[i] = '\0';
	return (str);
}

void	init_env(t_env **env, char *envp[])
{
	int i;

	i = count_env_amount(envp);
	(*env) = (t_env*)ft_memalloc(sizeof(t_env));
	(*env)->envp = (char**)ft_memalloc(sizeof(char*) * i);
	i = 0;
	while (envp[i])
	{
		(*env)->envp[i] = ft_strdup(envp[i]);
		//(*env)->names[i] = ft_strsubchar(envp[i], 0, '=');
		//(*env)->values[i] = ft_strdup(ft_strchr(envp[i], '=') + 1);
		i++;
	}
	(*env)->envp[i] = NULL;
}

void	print_envs(t_env *env)
{
	int i;

	i = 0;
	while (env->envp[i])
	{
		ft_printf("%s\n", env->envp[i]);
		i++;
	}
}

void	free_args(char **args)
{
	while (args)
		free(args++);
}

char	**split_line_args(char *line)
{
	char **args;

	args = (char**)ft_memalloc(sizeof(args));
	args = ft_strsplit(line, ' ');
	return (args);
}

char	*get_env_value(t_env *env, char *name)
{
	int len;
	int i;

	len = ft_strlen(name);
	i = 0;
	while (env->envp[i])
	{
		if (ft_strnequ(env->envp[i], name, len) && env->envp[i][len] == '=')
			return (ft_strchr(env->envp[i], '=') + 1);
		i++;
	}
	return ("");
}

char	*find_filepath(t_env *env, char *filename)
{
	char	*filepath;
	char	**paths;
	int		filename_len;
	int		i;

	paths = ft_strsplit(get_env_value(env, "PATH"), ':');
	filename_len = ft_strlen(filename);
	i = 0;
	while (paths[i])
	{
		filepath = (char*)ft_memalloc(ft_strlen(paths[i]) + filename_len + 2);
		ft_strcpy(filepath, paths[i]);
		ft_strcat(filepath, "/");
		ft_strcat(filepath, filename);
		if (access(filepath, F_OK) != -1)
			return (filepath);
		free(filepath);
		i++;
	}
	return (NULL);
}

void	wait_for_child(pid_t pid)
{
	int status;

	waitpid(pid, &status, WUNTRACED);
	while (!WIFEXITED(status) && !WIFSIGNALED(status))
		waitpid(pid, &status, WUNTRACED);
}

int		exec_cmd(t_env *env, char **args)
{
	char	*filepath;
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		if (!(filepath = find_filepath(env, args[0])))
		{
			print_error(ft_sprintf("Command not found: '%s'", args[0]));
			exit(1);
		}
		else
			execve(filepath, args, env->envp);
	}
	else if (pid < 0)
		print_error(ft_sprintf("Error forking"));
	else
		wait_for_child(pid);
	return (1);
}

void	handle_expansion(t_env *env, char **args)
{
	// Code this next to expand $env vars and ~ into args, by making new
	// string and changing $(foo)/~ to value it contains
}

int		check_cmd(t_env *env, char **args)
{
	if (!args[0])
		return (1);
	//if (ft_strchr(cmd->cmd[0], '$') || ft_strchr(cmd->cmd[0], '~'))
		//handle_expansion(env, cmd);
	return (exec_cmd(env, args));
}

void	loop_shell(t_env *env)
{
	char	*line;
	char	**args;
	int		loop;

	loop = 1;
	while (loop)
	{
		ft_printf("%s$> ", get_env_value(env, "USER"));
		if (get_next_line(0, &line) < 1)
			break;
		args = split_line_args(line);
		loop = check_cmd(env, args);
		free(args);
		free(line);
	}
}

int		main(int argc, char **argv, char *envp[])
{
	t_env *env;

	init_env(&env, envp);
	loop_shell(env);
	print_envs(env);
	return (0);
}