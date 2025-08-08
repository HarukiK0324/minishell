/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:56:44 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 00:56:45 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	remove_env(t_env **env_list, char *key)
{
	t_env	*prev;
	t_env	*current;

	prev = NULL;
	current = *env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*env_list = current->next;
			free(current->key);
			free(current->value);
			free(current);
			break ;
		}
		prev = current;
		current = current->next;
	}
}

int	exec_unset(t_token *argv, t_env *env_list)
{
	t_token	*tmp;

	tmp = argv->next;
	while (tmp)
	{
		remove_env(&env_list, tmp->value);
		tmp = tmp->next;
	}
	return (0);
}

int	exec_env(t_env *env_list)
{
	while (env_list)
	{
		if (env_list->value)
			printf("%s=%s\n", env_list->key, env_list->value);
		env_list = env_list->next;
	}
	return (0);
}

long	ft_atol(const char *str)
{
	int		i;
	int		sign;
	long	result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

int	exec_exit(t_token *argv)
{
	long	exit_code;

	printf("exit\n");
	if (argv->next)
	{
		if (!is_valid_long(argv->next->value))
		{
			write(2, "minishell: exit: ", 17);
			write(2, argv->next->value, ft_strlen(argv->next->value));
			write(2, ": numeric argument required\n", 29);
			exit(2);
		}
		if (argv->next->next)
		{
			write(2, "minishell: exit: too many arguments\n", 37);
			return (1);
		}
		exit_code = ft_atol(argv->next->value);
		exit((int)(exit_code & 0xFF));
	}
	exit(0);
}
