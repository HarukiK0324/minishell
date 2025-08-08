/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:03 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 02:07:21 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	pipe_executor1(int fd[2], t_node *ast, t_env *env_list, int *status)
{
	dup2(fd[1], STDOUT_FILENO);
	close(fd[0]);
	close(fd[1]);
	executor(ast, env_list, status);
	exit(*status);
}

void	pipe_executor2(int fd[2], t_node *ast, t_env *env_list, int *status)
{
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);
	close(fd[1]);
	executor(ast, env_list, status);
	exit(*status);
}

void	exec_pipe(t_node *ast, t_env *env_list, int *status)
{
	int		fd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status1;
	int		status2;

	if (pipe(fd) == -1)
		exec_error(status, "pipe");
	pid1 = fork();
	if (pid1 < 0)
		exec_error(status, "fork");
	if (pid1 == 0)
		pipe_executor1(fd, ast->lhs, env_list, status);
	pid2 = fork();
	if (pid2 < 0)
		exec_error(status, "fork");
	if (pid2 == 0)
		pipe_executor2(fd, ast->rhs, env_list, status);
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	handle_status(status2, status);
}

void	exec_cmd(t_env *env_list, t_cmd *cmd, int *status)
{
	pid_t	pid;
	int		wstatus;

	if (cmd->argv && is_builtin(cmd->argv->value))
		return (exec_builtin(env_list, cmd, status));
	pid = fork();
	if (pid < 0)
		exec_error(status, "fork");
	if (pid == 0)
		ft_execve(env_list, cmd);
	waitpid(pid, &wstatus, 0);
	handle_status(wstatus, status);
}

void	executor(t_node *ast, t_env *env_list, int *status)
{
	if (!ast)
		return ;
	if (ast->type == NODE_PIPE && g_status == 0)
		exec_pipe(ast, env_list, status);
	else if ((ast->type == NODE_AND_IF || ast->type == NODE_OR_IF)
		&& g_status == 0)
	{
		executor(ast->lhs, env_list, status);
		if (ast->type == NODE_AND_IF && *status == 0 && g_status == 0)
		{
			expander(ast->rhs, env_list, status);
			executor(ast->rhs, env_list, status);
		}
		else if (ast->type == NODE_OR_IF && *status != 0 && g_status == 0)
		{
			expander(ast->rhs, env_list, status);
			executor(ast->rhs, env_list, status);
		}
	}
	else if (ast->type == NODE_CMD && g_status == 0)
		exec_cmd(env_list, ast->cmd, status);
}
