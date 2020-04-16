/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadawi <sadawi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/15 12:28:40 by sadawi            #+#    #+#             */
/*   Updated: 2020/04/16 15:05:13 by sadawi           ###   ########.fr       */
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

int		check_names_match(char *var1, char *var2)
{
	int i;

	i = 0;
	while (var1[i] && var2[i])
	{
		if (var1[i] != var2[i])
			return (0);
		if (var1[i] == '=' && var2[i] == '=')
			return (1);
		i++;
	}
	return (0);
}

void	free_envp(char **envp)
{
	int i;

	i = 0;
	//while (envp[i])
		//free(envp[i++]);
	free(envp);
}

void	add_env(t_env *env, char *arg)
{
	char **new_envp;
	int i;

	new_envp = (char**)ft_memalloc(sizeof(char*)
	* (count_env_amount(env->envp) + 2));
	i = 0;
	while (env->envp[i])
	{
		if (check_names_match(env->envp[i], arg))
		{
			free(env->envp[i]);
			env->envp[i] = ft_strdup(arg);
			free_envp(new_envp);
			return ;
		}
		else
			new_envp[i] = env->envp[i];
		i++;
	}
	new_envp[i] = ft_strdup(arg);
	new_envp[i + 1] = NULL;
	free_envp(env->envp);
	env->envp = new_envp;
}

int		builtin_setenv(t_env *env, char **args)
{
	int i;

	i = 1;
	while (args[i])
	{
		if (ft_strchr(args[i], '='))
		{
			add_env(env, args[i]);
			print_error(ft_sprintf("%s added to enviroment!", args[i]));
		}
		else
		{
			print_error(ft_sprintf("'%s' could not be added.", args[i]));
			print_error(ft_sprintf("Correct format: NAME=value"));
		}
		i++;
	}
	return (1);
}

int		delete_env(t_env *env, char *arg)
{
	char **new_envp;
	int		len;
	int		i;
	int		deleted;

	new_envp = (char**)ft_memalloc(sizeof(char*)
	* (count_env_amount(env->envp)));
	len = ft_strlen(arg);
	deleted = 0;
	i = 0;
	while (env->envp[i])
	{
		if (!ft_strnequ(env->envp[i], arg, len))
			new_envp[i - deleted] = env->envp[i];
		else
			deleted = 1;
		i++;
	}
	if (deleted)
	{
		new_envp[i - deleted] = NULL;
		free_envp(env->envp);
		env->envp = new_envp;
		return (1);
	}
	free_envp(new_envp);
	return (0);
}

int		builtin_unsetenv(t_env *env, char **args)
{
	int i;

	i = 1;
	while (args[i])
	{
		if (delete_env(env, args[i]))
			print_error(ft_sprintf("%s deleted from enviroment!", args[i]));
		else
			print_error(ft_sprintf("'%s' not found.", args[i]));
		i++;
	}
	return (1);
}

char	*get_env_value(t_env *env, char *name)
{
	int len;
	int i;

	name = ft_strsubchar(name, 0, '$');
	len = ft_strlen(name);
	i = 0;
	while (env->envp[i])
	{
		if (ft_strnequ(env->envp[i], name, len) && env->envp[i][len] == '=')
		{
			free(name);
			return (ft_strchr(env->envp[i], '=') + 1);
		}
		i++;
	}
	free(name);
	return ("");
}

char	*store_oldpwd(t_env *env)
{
	char *path;

	path = (char*)ft_memalloc(PATH_MAX + 1);
	getcwd(path, PATH_MAX);
	return (path);
}

void	update_oldpwd(t_env *env, char *path)
{
	add_env(env, ft_strjoin("OLDPWD=", path));
	free(path);
}

void	update_pwd(t_env *env)
{
	char *path;

	path = (char*)ft_memalloc(PATH_MAX + 1);
	getcwd(path, PATH_MAX);
	add_env(env, ft_strjoin("PWD=", path));
	free(path);
}

int		builtin_cd(t_env *env, char **args)
{
	char *oldpwd;
	
	oldpwd = store_oldpwd(env);
	if (!args[1])
		chdir(get_env_value(env, "HOME"));
	else if (ft_strequ(args[1], "-"))
		chdir(get_env_value(env, "OLDPWD"));
	else
		chdir(args[1]);
	update_pwd(env);
	update_oldpwd(env, oldpwd);
	return (1);
}

void	init_builtins(t_env *env)
{
	char *builtin_names;
	int count;

	builtin_names = "echo exit env setenv unsetenv cd";
	env->builtin_names = ft_strsplit(builtin_names, ' ');
	count = 0;
	while (env->builtin_names[count])
		count++;
	env->builtin_funcs = (builtin_func**)ft_memalloc(sizeof(builtin_func*)
	* count);
	env->builtin_funcs[0] = &builtin_echo;
	env->builtin_funcs[1] = &builtin_exit;
	env->builtin_funcs[2] = &builtin_env;
	env->builtin_funcs[3] = &builtin_setenv;
	env->builtin_funcs[4] = &builtin_unsetenv;
	env->builtin_funcs[5] = &builtin_cd;
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

char	*find_filepath(t_env *env, char *filename)
{
	char	*filepath;
	char	**paths;
	int		filename_len;
	int		i;

	paths = ft_strsplit(get_env_value(env, "PATH"), ':');
	filename_len = ft_strlen(filename);
	i = 0;
	if (access(filename, F_OK) != -1)
			return (filename);
	while (paths[i])
	{
		filepath = (char*)ft_memalloc(ft_strlen(paths[i]) + filename_len + 2);
		ft_strcpy(filepath, paths[i]);
		ft_strcat(filepath, "/");
		ft_strcat(filepath, filename);
		//CHECK IF PROGRAM CAN BE EXECUTED, IF NOT DISPLAY PERMISSION ERROR
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

int		find_size_pointers(char *str, char *ptr)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] == *ptr)
			return (i);
		i++;
	}
	return (-1);
}

int		get_env_name_len(char *str)
{
	int i;

	i = 0;
	while (str[i] && str[i] != '$')
		i++;
	return (i);
}

char	*expand(t_env *env, char *str, char *ptr)
{
	char	*expanded_str;
	char	*value;
	int		len;

	if (*ptr == '~')
		expanded_str = ft_strjoin(get_env_value(env, "HOME"), ptr + 1);
	else if (*ptr == '$')
	{
		len = find_size_pointers(str, ptr);
		value = ft_strdup(get_env_value(env, ptr + 1));
		expanded_str = ft_strjoinfree(ft_strsub(str, 0, len), value);
		len = get_env_name_len(ptr + 1);
		expanded_str = ft_strjoinfree(expanded_str, ft_strdup(ptr + 1 + len));
	}
	return (expanded_str);
}

char	*find_dollar(const char *s)
{
	int i;

	i = 0;
	while (s[i])
	{
		if (s[i] == '$')
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
		if (args[i][0] == '~')
			args[i] = expand(env, args[i], &args[i][0]);
		i++;
	}
	i = 0;
	while (args[i])
	{
		if ((ptr = find_dollar(args[i])) && ptr + 1)
			args[i] = expand(env, args[i], ptr);
		else
			i++;
	}
	// Code this next to expand $env vars and ~ into args, by making new
	// string and changing $(foo)/~ to value it contains
	// HANDLE ~ AND $ SEPERATELY, FIRST ~ IN STRING BEGINNING, THEN $ POSSIBLY
	// IN SAME STRING. STRING CAN ALSO CONTAIN SEVERAL $, EX: "$HOME$HOME"
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

char	*get_pwd_base(t_env *env)
{
	char *pwd;

	pwd = get_env_value(env, "PWD");
	return (ft_strrchr(pwd, '/') + 1);
}

void	loop_shell(t_env *env)
{
	char	*line;
	char	**args;
	int		loop;

	loop = 1;
	while (loop)
	{
		ft_printf("%s@", get_env_value(env, "USER"));
		ft_printf(BOLDBLUE "%s " RESET,get_pwd_base(env));
		ft_printf("$> ",get_pwd_base(env));
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