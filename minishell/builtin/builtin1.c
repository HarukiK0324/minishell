/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:56:44 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 20:09:30 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_env_list(t_env *env_list)
{
	while (env_list)
	{
		if (env_list->value)
			printf("%s=%s\n", env_list->key, env_list->value);
		env_list = env_list->next;
	}
}

int	env_no_path_error(char *cmd)
{
	write(2, "env: '", 6);
	write(2, cmd, ft_strlen(cmd));
	write(2, "': No such file or directory\n", 29);
	return (127);
}

int	exec_env(t_token *argv, t_env *env_list)
{
	char	*path;
	char	**args;
	char	**environ;

	if (!argv->next)
		return (print_env_list(env_list), 0);
	path = get_path(argv->next->value, env_list);
	if (!path)
		return (env_no_path_error(argv->next->value));
	args = to_list(argv->next);
	environ = env_to_environ(env_list);
	if (!args || !environ)
		return (free(path), free_str_list(args), free_str_list(environ), 1);
	execve(path, args, environ);
	free_all_arg(path, args, environ);
	return (127);
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

void	exec_exit(t_shell *shell, t_token *argv)
{
	long	exit_code;

	if (argv->next)
	{
		printf("exit\n");
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
			*shell->status = 1;
		}
		exit_code = ft_atol(argv->next->value);
		exit((int)(exit_code & 0xFF));
	}
	ft_exit(shell, shell->env_list, *shell->status);
}
