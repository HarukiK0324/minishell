/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 13:02:50 by hkasamat          #+#    #+#             */
/*   Updated: 2025/03/29 21:01:17 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_line_from_buffer(char *line, int *fd)
{
	char	*buffer;
	int		bytes_read;

	buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	bytes_read = read(*fd, buffer, BUFFER_SIZE);
	if (bytes_read <= 0)
	{
		free(buffer);
		if (bytes_read < 0)
			return (NULL);
		*fd = -1;
		return (line);
	}
	buffer[bytes_read] = '\0';
	line = ft_strjoin(line, buffer);
	free(buffer);
	return (line);
}

char	*get_first_line(char *line)
{
	char	*first_line;
	int		i;

	if (find_newline(line) == -1)
		first_line = (char *)malloc(ft_strlen(line) + 1);
	else
		first_line = (char *)malloc(find_newline(line) + 2);
	if (first_line == NULL)
		return (NULL);
	i = 0;
	while (line[i] != '\n' && line[i] != '\0')
	{
		first_line[i] = line[i];
		i++;
	}
	if (line[i] == '\n')
	{
		first_line[i] = '\n';
		i++;
	}
	first_line[i] = '\0';
	return (first_line);
}

char	*update_line(char *line, char **first_line)
{
	int		i;
	int		j;
	char	*new_line;

	i = find_newline(line) + 1;
	if ((i == ft_strlen(line)) || find_newline(line) == -1)
	{
		free(line);
		return (NULL);
	}
	new_line = (char *)malloc(ft_strlen(line) - i + 1);
	if (!new_line)
	{
		free(*first_line);
		*first_line = NULL;
		return (NULL);
	}
	j = 0;
	while (line[i] != '\0')
		new_line[j++] = line[i++];
	new_line[j] = '\0';
	free(line);
	return (new_line);
}

char	*get_next_line(int fd)
{
	static char	*line;
	char		*first_line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	while (find_newline(line) == -1)
	{
		line = get_line_from_buffer(line, &fd);
		if (!line)
			return (NULL);
		else if (fd == -1)
			break ;
	}
	first_line = get_first_line(line);
	line = update_line(line, &first_line);
	return (first_line);
}
