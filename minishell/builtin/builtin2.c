/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:56:47 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 20:21:45 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_export(t_env *env_list)
{
	t_env	**arr;
	int		count;

	if (!env_list)
		return ;
	count = count_env(env_list);
	arr = create_env_array(env_list, count);
	if (!arr)
		return ;
	sort_env_array(arr, count);
	print_sorted_env(arr, count);
	free(arr);
}

void	print_export_error(char *value)
{
	write(2, "minishell: export: `", 20);
	write(2, value, ft_strlen(value));
	write(2, "': not a valid identifier\n", 27);
}

int	handle_export_with_equals(t_token *tmp, t_env *env_list)
{
	char	*eq;
	char	*key;
	int		is_append;

	eq = ft_strchr(tmp->value, '=');
	key = ft_strndup(tmp->value, eq - tmp->value);
	is_append = (key && ft_strlen(key) > 0 && key[ft_strlen(key) - 1] == '+');
	if (is_append)
		key[ft_strlen(key) - 1] = '\0';
	if (!is_valid_identifier(key))
	{
		print_export_error(tmp->value);
		free(key);
		return (1);
	}
	if (is_append)
		handle_append_export(env_list, key, eq + 1);
	else
		update_env(env_list, key, eq + 1);
	free(key);
	return (0);
}

int	handle_export_without_equals(t_token *tmp, t_env *env_list)
{
	if (!is_valid_identifier(tmp->value))
	{
		print_export_error(tmp->value);
		return (1);
	}
	update_env(env_list, tmp->value, NULL);
	return (0);
}

int	exec_export(t_token *argv, t_env *env_list)
{
	t_token	*tmp;
	int		status;

	tmp = argv->next;
	if (!tmp)
		return (print_export(env_list), 0);
	status = 0;
	while (tmp)
	{
		if (ft_strchr(tmp->value, '='))
			status |= handle_export_with_equals(tmp, env_list);
		else
			status |= handle_export_without_equals(tmp, env_list);
		tmp = tmp->next;
	}
	return (status);
}
