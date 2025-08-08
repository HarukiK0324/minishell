#include "minishell.h"

char	*find_pwd_value(t_env *env_list)
{
	t_env	*pwd_env;

	pwd_env = env_list;
	while (pwd_env && ft_strcmp(pwd_env->key, "PWD") != 0)
		pwd_env = pwd_env->next;
	if (!pwd_env || !pwd_env->value)
		return (NULL);
	return (pwd_env->value);
}

void	copy_parent_path(char *parent, char *pwd, int len)
{
	int	j;

	if (len == 0)
	{
		parent[0] = '/';
		parent[1] = '\0';
	}
	else
	{
		j = -1;
		while (++j < len)
			parent[j] = pwd[j];
		parent[j] = '\0';
	}
}

char	*get_parent_from_pwd(t_env *env_list)
{
	char	*pwd;
	char	*parent;
	int		i;

	pwd = find_pwd_value(env_list);
	if (!pwd)
		return (NULL);
	parent = malloc(ft_strlen(pwd) + 1);
	if (!parent)
		return (NULL);
	i = ft_strlen(pwd) - 1;
	while (i > 0 && pwd[i] == '/')
		i--;
	while (i > 0 && pwd[i] != '/')
		i--;
	copy_parent_path(parent, pwd, i);
	return (parent);
}

char	*get_cd_path(t_token *argv, t_env *env_list)
{
	t_env	*home_env;

	if (!argv->next || !argv->next->value)
	{
		home_env = find_env(env_list, "HOME");
		if (!home_env || !home_env->value)
		{
			write(2, "minishell: cd: HOME not set\n", 28);
			return (NULL);
		}
		return (home_env->value);
	}
	return (argv->next->value);
}

void	update_pwd(t_env *env_list, char *new_pwd)
{
	t_env	*pwd_env;
	t_env	*oldpwd_env;
	char	*old_pwd;

	pwd_env = env_list;
	while (pwd_env && ft_strcmp(pwd_env->key, "PWD") != 0)
		pwd_env = pwd_env->next;
	oldpwd_env = env_list;
	while (oldpwd_env && ft_strcmp(oldpwd_env->key, "OLDPWD") != 0)
		oldpwd_env = oldpwd_env->next;
	if (pwd_env && pwd_env->value)
	{
		old_pwd = pwd_env->value;
		pwd_env->value = ft_strdup(new_pwd);
		if (oldpwd_env)
		{
			free(oldpwd_env->value);
			oldpwd_env->value = old_pwd;
		}
		else
			free(old_pwd);
	}
}
