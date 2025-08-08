/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin7.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:00 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 03:57:21 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	only_contains(char *str, char *chars)
{
	int	i;
	int	j;

	i = 1;
	while (str[i] != '\0')
	{
		j = 0;
		while (chars[j] != '\0')
		{
			if (str[i] == chars[j])
				break ;
			j++;
		}
		if (chars[j] == '\0')
			return (0);
		i++;
	}
	return (1);
}

int	exec_echo(t_token *argv)
{
	int		newline;
	t_token	*tmp;

	newline = 1;
	tmp = argv->next;
	while (tmp && tmp->value[0] == '-' && tmp->value[1] != '\0'
		&& only_contains(tmp->value, "n"))
	{
		newline = 0;
		tmp = tmp->next;
	}
	while (tmp)
	{
		printf("%s", tmp->value);
		tmp = tmp->next;
		if (tmp)
			printf(" ");
	}
	if (newline)
		printf("\n");
	return (0);
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
