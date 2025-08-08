#include "minishell.h"

char	*get_path_from_env(char *argv, t_env *env_list)
{
	char	**paths;
	int		i;
	char	*path;

	i = 0;
	while (env_list && ft_strcmp(env_list->key, "PATH") != 0)
		env_list = env_list->next;
	if (!env_list)
		return (NULL);
	paths = ft_split(env_list->value, ':');
	path = NULL;
	while (paths && paths[i])
	{
		path = ft_access(paths[i++], argv);
		if (path)
			break ;
	}
	free_str_list(paths);
	if (path)
		return (path);
	return (NULL);
}

char	*get_path(char *cmd, t_env *env_list)
{
	if (ft_strchar(cmd, '/') != ft_strlen(cmd))
		return (ft_strdup(cmd));
	return (get_path_from_env(cmd, env_list));
}

char	**to_list(t_token *argv)
{
	char	**list;
	size_t	i;

	if (!argv)
		return (NULL);
	list = (char **)malloc(sizeof(char *) * (ft_token_size(argv) + 1));
	if (!list)
		return (perror("malloc"), NULL);
	i = 0;
	while (argv)
	{
		list[i] = ft_strdup(argv->value);
		if (!list[i])
			return (perror("strdup"), free_argv(list, i), NULL);
		argv = argv->next;
		i++;
	}
	list[i] = NULL;
	return (list);
}

char	*ft_env_join(t_env *env_list)
{
	char	*env;
	int		i;
	int		j;

	if (!env_list)
		return (NULL);
	env = (char *)malloc(ft_strlen(env_list->key) + ft_strlen(env_list->value)
			+ 2);
	if (!env)
		return (perror("malloc"), NULL);
	i = -1;
	while (env_list->key[++i] != '\0')
		env[i] = env_list->key[i];
	env[i++] = '=';
	j = 0;
	while (env_list->value[j] != '\0')
		env[i++] = env_list->value[j++];
	env[i] = '\0';
	return (env);
}

char	**env_to_environ(t_env *env_list)
{
	char	**environ;
	size_t	i;

	if (!env_list)
		return (NULL);
	environ = (char **)malloc(sizeof(char *) * (ft_env_size(env_list) + 1));
	if (!environ)
		return (perror("malloc"), NULL);
	i = 0;
	while (env_list)
	{
		environ[i] = ft_env_join(env_list);
		if (!environ[i])
			return (free_argv(environ, i), NULL);
		env_list = env_list->next;
		i++;
	}
	environ[i] = NULL;
	return (environ);
}
