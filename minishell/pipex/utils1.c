/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 16:49:41 by hkasamat          #+#    #+#             */
/*   Updated: 2025/03/29 22:31:52 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*get_path(char *argv, char *environ[])
{
	char	**paths;
	int		i;
	char	*path;

	i = 0;
	while (*(++environ) != NULL)
	{
		if (ft_strncmp(*environ, "PATH=", 5) == 0)
			break ;
	}
	if (*environ == NULL)
		cmd_error("zsh: command not found: ", argv);
	paths = ft_split(*environ + 5, ':');
	path = NULL;
	while (paths && paths[i] != NULL && path == NULL)
		path = ft_access(paths[i++], argv);
	i = 0;
	while (paths[i])
		free(paths[i++]);
	free(paths);
	if (path == NULL)
		cmd_error("zsh: command not found: ", argv);
	return (path);
}

void	ft_exec(char *cmd, char *environ[])
{
	int		i;
	char	**args;
	char	*path;

	i = 0;
	args = ft_split(cmd, ' ');
	if (access(args[0], F_OK) == 0)
		path = args[0];
	else
		path = get_path(args[0], environ);
	if (execve(path, args, NULL) == -1)
	{
		write(2, "zsh: command not found: ", 24);
		write(2, args[0], ft_strlen(args[0]));
		write(2, "\n", 1);
		while (args && args[i])
		{
			free(args[i]);
			i++;
		}
		free(args);
		if (path != NULL)
			free(path);
		exit(0);
	}
}

int	open_file(char *argv, int i)
{
	int	file;

	file = 0;
	if (i == 0)
		file = open(argv, O_RDONLY);
	else if (i == 1)
		file = open(argv, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (i == 2)
		file = open(argv, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (file == -1)
	{
		write(2, "zsh: permission denied: ", 24);
		write(2, argv, ft_strlen(argv));
		write(2, "\n", 1);
	}
	return (file);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

void	end_read(char *line)
{
	free(line);
	exit(EXIT_SUCCESS);
}
