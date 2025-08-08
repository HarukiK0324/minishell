#include "minishell.h"

size_t	ft_token_size(t_token *tokens)
{
	size_t	size;

	size = 0;
	while (tokens)
	{
		size++;
		tokens = tokens->next;
	}
	return (size);
}

size_t	ft_env_size(t_env *env_list)
{
	size_t	size;

	size = 0;
	while (env_list)
	{
		size++;
		env_list = env_list->next;
	}
	return (size);
}

void	free_argv(char **argv, size_t i)
{
	size_t	index;

	index = 0;
	while (index < i)
	{
		free(argv[index]);
		index++;
	}
	free(argv);
}

void	free_str_list(char **list)
{
	size_t	i;

	if (!list)
		return ;
	i = 0;
	while (list[i])
		free(list[i++]);
	free(list);
}

char	*ft_access(char *path, char *cmd)
{
	char	*full_path;
	int		i;
	int		j;

	full_path = (char *)malloc(ft_strlen(path) + ft_strlen(cmd) + 2);
	if (!full_path)
		return (perror("malloc"), NULL);
	i = -1;
	while (path && path[++i] != '\0')
		full_path[i] = path[i];
	full_path[i++] = '/';
	j = 0;
	while (cmd && cmd[j] != '\0')
		full_path[i++] = cmd[j++];
	full_path[i] = '\0';
	if (access(full_path, F_OK) == -1)
		return (free(full_path), NULL);
	return (full_path);
}
