/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:13 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 01:28:53 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*trim_double_quote(char *str, size_t *j, t_env *env_list, int *status)
{
	size_t	i;

	i = *j + 1;
	while (str && str[i] != '"')
	{
		if (str[i] == '$')
			str = parse_env_var(str, &i, env_list, status);
		else
			i++;
	}
	return (trim_quote(str, j, '"'));
}

char	*parse_env_var(char *str, size_t *j, t_env *env_list, int *status)
{
	size_t	i;
	size_t	k;
	char	*env_var;

	if (str && str[*j + 1] == '?')
		return (replace_status(str, j, status));
	i = 0;
	while (str && is_numchar(str[*j + 1 + i]))
		i++;
	if (i > 0 && str && is_char(str[*j + 1]))
	{
		env_var = (char *)malloc(i + 1);
		if (!env_var)
			return (perror("malloc"), NULL);
		k = -1;
		while (++k < i)
			env_var[k] = str[*j + 1 + k];
		env_var[k] = '\0';
		str = replace_env_var(str, j, i, find_env_value(env_var, env_list));
		free(env_var);
	}
	else
		(*j)++;
	return (str);
}

char	*find_env_value(char *env_var, t_env *env_list)
{
	char	*value;

	value = NULL;
	while (env_list)
	{
		if (ft_strcmp(env_var, env_list->key) == 0)
		{
			value = env_list->value;
			break ;
		}
		env_list = env_list->next;
	}
	return (value);
}
