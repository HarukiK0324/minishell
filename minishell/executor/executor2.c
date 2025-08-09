/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 02:04:31 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 15:46:10 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_error(int *status, char *str)
{
	*status = 2;
	return (perror(str));
}

int	is_builtin(char *cmd)
{
	if (ft_strcmp(cmd, "cd") == 0 || ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "exit") == 0 || ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0)
		return (1);
	return (0);
}

void	run_builtin(t_env *env_list, t_cmd *cmd, int *status)
{
	if (ft_strcmp(cmd->argv->value, "cd") == 0)
		*status = exec_cd(cmd->argv, env_list);
	else if (ft_strcmp(cmd->argv->value, "echo") == 0)
		*status = exec_echo(cmd->argv);
	else if (ft_strcmp(cmd->argv->value, "exit") == 0)
		*status = exec_exit(cmd->argv);
	else if (ft_strcmp(cmd->argv->value, "export") == 0)
		*status = exec_export(cmd->argv, env_list);
	else if (ft_strcmp(cmd->argv->value, "pwd") == 0)
		*status = exec_pwd();
	else if (ft_strcmp(cmd->argv->value, "unset") == 0)
		*status = exec_unset(cmd->argv, env_list);
	else if (ft_strcmp(cmd->argv->value, "env") == 0)
		*status = exec_env(cmd->argv, env_list);
}

void	handle_builtin_fd(t_cmd *cmd, int *status, int original_stdin,
		int original_stdout)
{
	*status = 2;
	if (cmd->fd_in != 0)
		close(cmd->fd_in);
	if (cmd->fd_out != 1)
		close(cmd->fd_out);
	close(original_stdin);
	close(original_stdout);
}

void	handle_status(int wstatus, int *status)
{
	if (WIFEXITED(wstatus))
		*status = WEXITSTATUS(wstatus);
	else if (WTERMSIG(wstatus) == SIGQUIT)
	{
		write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
		g_status = 3;
	}
	else if (WTERMSIG(wstatus) == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		g_status = 2;
	}
	if (g_status != 0)
		*status = 128 + g_status;
}
