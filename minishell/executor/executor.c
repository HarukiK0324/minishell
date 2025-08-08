/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:03 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 00:57:04 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_pipe(t_node *ast, t_env *env_list, int *status)
{
	int		fd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status1;
	int		status2;

	if (pipe(fd) == -1)
	{
		*status = -1;
		return (perror("pipe"));
	}
	pid1 = fork();
	if (pid1 < 0)
	{
		*status = -1;
		return (perror("fork"));
	}
	if (pid1 == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		executor(ast->lhs, env_list, status);
		exit(*status);
	}
	pid2 = fork();
	if (pid2 < 0)
	{
		*status = -1;
		return (perror("fork"));
	}
	if (pid2 == 0)
	{
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		close(fd[1]);
		executor(ast->rhs, env_list, status);
		exit(*status);
	}
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	if (WIFEXITED(status2))
		*status = WEXITSTATUS(status2);
	else if (WTERMSIG(status2) == SIGQUIT)
	{
		write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
		g_status = 3;
	}
	else if (WTERMSIG(status2) == SIGINT)
	{
		write(STDOUT_FILENO, "\b\n", 1);
		g_status = 2;
	}
	if (g_status != 0)
		*status = 128 + g_status;
}

void	exec_cmd(t_env *env_list, t_cmd *cmd, int *status)
{
	pid_t	pid;
	int		wstatus;

	if (cmd->argv && is_builtin(cmd->argv->value))
		return (exec_builtin(env_list, cmd, status));
	pid = fork();
	if (pid < 0)
	{
		*status = -1;
		return (perror("fork"));
	}
	if (pid == 0)
		ft_execve(env_list, cmd);
	waitpid(pid, &wstatus, 0);
	if (WIFEXITED(wstatus))
		*status = WEXITSTATUS(wstatus);
	else if (WTERMSIG(wstatus) == SIGQUIT)
	{
		write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
		g_status = 3;
	}
	else if (WTERMSIG(wstatus) == SIGINT)
	{
		write(STDOUT_FILENO, "\b\n", 1);
		g_status = 2;
	}
	if (g_status != 0)
		*status = 128 + g_status;
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
