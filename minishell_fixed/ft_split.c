/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 22:41:22 by hkasamat          #+#    #+#             */
/*   Updated: 2025/07/27 20:50:05 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	count_words(const char *s, char c)
{
	size_t	count;
	size_t	in_word;

	count = 0;
	in_word = 0;
	while (*s != '\0')
	{
		if (*s == c)
			in_word = 0;
		else if (!in_word)
		{
			in_word = 1;
			count++;
		}
		s++;
	}
	return (count);
}

char	*substring(char const *s, size_t index, char c)
{
	size_t	len;
	size_t	i;
	char	*str;

	len = 0;
	i = 0;
	while (s[index + len] != '\0' && s[index + len] != c)
		len++;
	str = (char *)malloc(len + 1);
	if (str == NULL)
		return (NULL);
	while (i < len)
	{
		str[i] = s[index + i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

void	free_all(char **arr, size_t i)
{
	size_t	index;

	index = 0;
	while (index < i)
	{
		free(arr[index]);
		index++;
	}
	free(arr);
}

char	**ft_split(char const *s, char c)
{
	char	**arr;
	size_t	i;
	size_t	index;

	i = 0;
	index = 0;
	arr = (char **)malloc((count_words(s, c) + 1) * sizeof(char *));
	while (i < count_words(s, c))
	{
		while (s[index] == c)
			index++;
		arr[i] = substring(s, index, c);
		if (arr[i] == NULL)
		{
			free_all(arr, i);
			return (NULL);
		}
		index += ft_strlen(arr[i]);
		i++;
	}
	arr[i] = NULL;
	return (arr);
}
