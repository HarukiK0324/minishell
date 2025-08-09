/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin5.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:56:54 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 15:44:58 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_cd_parent_dir(t_env *env_list)
{
	char	*parent_path;

	parent_path = get_parent_from_pwd(env_list);
	if (!parent_path || chdir(parent_path) == -1)
	{
		write(2, "minishell: cd: ", 15);
		perror("..");
		if (parent_path)
			free(parent_path);
		return (1);
	}
	update_pwd(env_list, parent_path);
	free(parent_path);
	return (0);
}

int	handle_cd_regular(char *path, t_env *env_list)
{
	char	*new_pwd;

	if (chdir(path) == -1)
	{
		write(2, "minishell: cd: ", 15);
		perror(path);
		return (1);
	}
	new_pwd = getcwd(NULL, 0);
	if (new_pwd)
	{
		update_pwd(env_list, new_pwd);
		free(new_pwd);
	}
	return (0);
}

int	handle_cd_oldpwd(t_env *env_list)
{
	t_env	*oldpwd_env;
	char	*oldpwd;
	int		result;

	oldpwd_env = env_list;
	while (oldpwd_env && ft_strcmp(oldpwd_env->key, "OLDPWD") != 0)
		oldpwd_env = oldpwd_env->next;
	if (!oldpwd_env || !oldpwd_env->value)
	{
		write(2, "minishell: cd: OLDPWD not set\n", 31);
		return (1);
	}
	oldpwd = ft_strdup(oldpwd_env->value);
	result = handle_cd_regular(oldpwd, env_list);
	if (result == 0)
		printf("%s\n", oldpwd);
	free(oldpwd);
	return (result);
}

int	exec_cd(t_token *argv, t_env *env_list)
{
	char	*path;

	if (argv->next && argv->next->next)
	{
		write(2, "minishell: cd: too many arguments\n", 35);
		return (1);
	}
	path = get_cd_path(argv, env_list);
	if (!path)
		return (1);
	if (ft_strcmp(path, "..") == 0)
		return (handle_cd_parent_dir(env_list));
	if (ft_strcmp(path, "-") == 0)
		return (handle_cd_oldpwd(env_list));
	return (handle_cd_regular(path, env_list));
}

int	exec_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	else
	{
		perror("minishell: pwd");
		return (1);
	}
}
