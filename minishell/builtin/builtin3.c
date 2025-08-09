/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:56:49 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 15:44:56 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_env(t_env *env_list)
{
	int	count;

	count = 0;
	while (env_list)
	{
		count++;
		env_list = env_list->next;
	}
	return (count);
}

void	swap_env(t_env **a, t_env **b)
{
	char	*tmp_key;
	char	*tmp_value;

	tmp_key = (*a)->key;
	tmp_value = (*a)->value;
	(*a)->key = (*b)->key;
	(*a)->value = (*b)->value;
	(*b)->key = tmp_key;
	(*b)->value = tmp_value;
}

void	sort_env_array(t_env **arr, int n)
{
	int	i;
	int	j;

	i = 0;
	while (i < n - 1)
	{
		j = 0;
		while (j < n - i - 1)
		{
			if (ft_strcmp(arr[j]->key, arr[j + 1]->key) > 0)
				swap_env(&arr[j], &arr[j + 1]);
			j++;
		}
		i++;
	}
}

t_env	**create_env_array(t_env *env_list, int count)
{
	t_env	**arr;
	t_env	*tmp;
	int		i;

	arr = malloc(sizeof(t_env *) * count);
	if (!arr)
		return (NULL);
	tmp = env_list;
	i = 0;
	while (tmp)
	{
		arr[i++] = tmp;
		tmp = tmp->next;
	}
	return (arr);
}

void	print_sorted_env(t_env **arr, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (arr[i]->value)
			printf("declare -x %s=\"%s\"\n", arr[i]->key, arr[i]->value);
		else
			printf("declare -x %s\n", arr[i]->key);
		i++;
	}
}
