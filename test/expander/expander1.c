/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:11 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 01:27:55 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*str_trim(char *str, size_t *j, size_t i)
{
	char	*new_str;
	size_t	k;

	if (!str || *j < 0 || i < 0 || (*j + i >= ft_strlen(str)))
		return (perror("str_trim failed"), NULL);
	new_str = (char *)malloc(ft_strlen(str) - i);
	if (!new_str)
		return (perror("malloc"), NULL);
	k = 0;
	while (k < *j)
	{
		new_str[k] = str[k];
		k++;
	}
	while (str[k + 1 + i] != '\0')
	{
		new_str[k] = str[k + 1 + i];
		k++;
	}
	new_str[k] = '\0';
	free(str);
	return (new_str);
}

char	*str_insert(char *str, size_t *j, char *value)
{
	char	*new_str;
	size_t	k;
	size_t	i;

	if (!str || !value)
		return (perror("str_insert failed"), NULL);
	new_str = (char *)malloc(ft_strlen(str) + ft_strlen(value) + 1);
	if (!new_str)
		return (perror("malloc"), NULL);
	i = -1;
	while (++i < *j)
		new_str[i] = str[i];
	k = -1;
	while (value[++k] != '\0')
		new_str[i + k] = value[k];
	while (str[i] != '\0')
	{
		new_str[i + k] = str[i];
		i++;
	}
	new_str[i + k] = '\0';
	(*j) += k;
	free(str);
	return (new_str);
}

char	*replace_env_var(char *str, size_t *j, size_t i, char *value)
{
	str = str_trim(str, j, i);
	if (str && value && ft_strlen(value))
		str = str_insert(str, j, value);
	return (str);
}

char	*replace_status(char *str, size_t *j, int *status)
{
	char	*status_str;

	status_str = to_str(*status);
	if (!status_str)
		return (NULL);
	str = str_trim(str, j, 1);
	if (str)
		str = str_insert(str, j, status_str);
	free(status_str);
	return (str);
}

char	*trim_quote(char *str, size_t *j, char c)
{
	size_t	i;
	char	*new_str;

	i = -1;
	new_str = (char *)malloc(ft_strlen(str) - 1);
	if (!new_str)
		return (perror("malloc"), NULL);
	while (++i < *j)
		new_str[i] = str[i];
	while (str[i + 1] != c)
	{
		new_str[i] = str[i + 1];
		(*j)++;
		i++;
	}
	while (str[i + 2] != '\0')
	{
		new_str[i] = str[i + 2];
		i++;
	}
	new_str[i] = '\0';
	free(str);
	return (new_str);
}
