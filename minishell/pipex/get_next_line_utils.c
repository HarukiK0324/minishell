/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 14:38:28 by hkasamat          #+#    #+#             */
/*   Updated: 2025/03/29 21:01:14 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	find_newline(char *line)
{
	int	i;

	i = 0;
	if (line == NULL)
		return (-1);
	while (line[i] != '\0')
	{
		if (line[i] == '\n')
			return (i);
		i++;
	}
	return (-1);
}

char	*ft_strjoin(char *line, char *buffer)
{
	char	*new_line;
	int		i;
	int		j;

	new_line = (char *)malloc(ft_strlen(line) + ft_strlen(buffer) + 1);
	if (!new_line)
		return (NULL);
	i = 0;
	j = 0;
	while (line && line[i] != '\0')
		new_line[i++] = line[j++];
	j = 0;
	while (buffer[j] != '\0')
		new_line[i++] = buffer[j++];
	new_line[i] = '\0';
	if (line)
		free(line);
	return (new_line);
}
