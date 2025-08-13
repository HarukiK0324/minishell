/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:27 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 00:57:27 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	if (s == NULL)
		return (0);
	while (s[i] != '\0')
		i++;
	return (i);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(const unsigned char *)s1 - *(const unsigned char *)s2);
}

int	check_quote(char *input)
{
	while (*input != '\0')
	{
		if (*input == '\"')
		{
			input++;
			while (*input != '\0' && *input != '\"')
				input++;
			if (*input == '\0')
				return (-1);
		}
		else if (*input == '\'')
		{
			input++;
			while (*input != '\0' && *input != '\'')
				input++;
			if (*input == '\0')
				return (-1);
		}
		input++;
	}
	return (1);
}

char	*append(char *s1, char *s2, char c)
{
	char	*result;
	size_t	len1;
	size_t	len2;
	int		i;
	int		j;

	if (!s1 || !s2)
		return (perror("append failed"), NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	result = (char *)malloc(len1 + len2 + 2);
	if (!result)
		return (perror("malloc failed"), NULL);
	i = -1;
	while (s1 && s1[++i] != '\0')
		result[i] = s1[i];
	result[i++] = c;
	j = -1;
	while (s2 && s2[++j] != '\0')
		result[i + j] = s2[j];
	result[i + j] = '\0';
	free(s1);
	free(s2);
	return (result);
}

void	free_env(t_env *env_list)
{
	t_env	*temp;

	while (env_list)
	{
		temp = env_list;
		env_list = env_list->next;
		free(temp->key);
		free(temp->value);
		free(temp);
	}
}
