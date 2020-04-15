/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadawi <sadawi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/15 12:28:40 by sadawi            #+#    #+#             */
/*   Updated: 2020/04/15 22:40:18 by sadawi           ###   ########.fr       */
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

int		builtin_echo(t_env *env, char **args)
{
	int i;

	i = 1;
	while (args[i])
	{
		ft_printf("%s", args[i]);
		if (args[i + 1])
			ft_printf(" ");
		i++;
	}
	ft_printf("\n");
}

int		builtin_exit(t_env *env, char **args)
{
	//maybe should free memory here??
	exit(0);
}

int		builtin_env(t_env *env, char **args)
{
	int i;

	i = 0;
	while (env->envp[i])
		ft_printf("%s\n", env->envp[i++]);
}

/*int		builtin_setenv(t_env *env, char **args)
{
	char **new_envp;
	int i;

	i = 0;
	while (env->envp[i])
		if (ft_strequ(envp[i], args[0])
}*/

void	init_builtins(t_env *env)
{
	char *builtin_names;
	int count;

	builtin_names = "echo exit env setenv";
	env->builtin_names = ft_strsplit(builtin_names, ' ');
	count = 0;
	while (env->builtin_names[count])
		count++;
	env->builtin_funcs = (builtin_func**)ft_memalloc(sizeof(builtin_func*) * count);
	env->builtin_funcs[0] = &builtin_echo;
	env->builtin_funcs[1] = &builtin_exit;
	env->builtin_funcs[2] = &builtin_env;
	//env->builtin_funcs[3] = &builtin_setenv;
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
		i++;
	}
	(*env)->envp[i] = NULL;
	init_builtins(*env);
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
	args = ft_strsplitws(line);
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
		if (ft_strstr(filename, paths[i]))
			return (filename);
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

char	*expand(t_env *env, char *ptr)
{
	char *expanded_str;

	if (*ptr == '~')
		expanded_str = ft_strjoin(get_env_value(env, "HOME"), ptr + 1);
	else if (*ptr == '$')
		expanded_str = get_env_value(env, ptr + 1);
	return (expanded_str);
}

char	*find_expansion(const char *s)
{
	int i;

	i = 0;
	while (s[i])
	{
		if (s[i] == '$' || s[i] == '~')
			return ((char*)&s[i]);
		i++;
	}
	return (NULL);
}

void	handle_expansion(t_env *env, char **args)
{
	char *ptr;
	int i;

	i = 0;
	while (args[i])
	{
		if ((ptr = find_expansion(args[i])))
			args[i] = expand(env, ptr);
		i++;
	}
	// Code this next to expand $env vars and ~ into args, by making new
	// string and changing $(foo)/~ to value it contains
}

int		handle_builtins(t_env *env, char **args)
{
	int i;

	i = 0;
	while (env->builtin_names[i])
	{
		if (ft_strequ(env->builtin_names[i], args[0]))
			return (env->builtin_funcs[i](env, args));
		i++;
	}
	return (0);
}

int		check_cmd(t_env *env, char **args)
{
	if (!args[0])
		return (1);
	handle_expansion(env, args);
	if (handle_builtins(env, args))
		return (1);
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