/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:03 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/16 13:36:19 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	pipe_executor1(int fd[2], t_shell *shell, t_node *ast)
{
	dup2(fd[1], STDOUT_FILENO);
	close(fd[0]);
	close(fd[1]);
	executor(shell, ast, shell->env_list, shell->status);
	exit(*shell->status);
}

void	pipe_executor2(int fd[2], t_shell *shell, t_node *ast)
{
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);
	close(fd[1]);
	executor(shell, ast, shell->env_list, shell->status);
	exit(*shell->status);
}

void	exec_pipe(t_shell *shell, t_node *ast, int *status)
{
	int		fd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status1;
	int		status2;

	if (pipe(fd) == -1)
		exec_error(status, "pipe");
	signal_pipe_hold(ast);
	pid1 = fork();
	if (pid1 < 0)
		return (pipe_cleanup(fd, ast, status));
	if (pid1 == 0)
		pipe_executor1(fd, shell, ast->lhs);
	pid2 = fork();
	if (pid2 < 0)
		return (waitpid(pid1, NULL, 0), pipe_cleanup(fd, ast, status));
	if (pid2 == 0)
		pipe_executor2(fd, shell, ast->rhs);
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	signal_pipe_revert(ast);
	handle_status(status2, status);
}

void	exec_cmd(t_shell *shell, t_env *env_list, t_cmd *cmd, int *status)
{
	pid_t	pid;
	int		wstatus;
	void	(*old_sigint)(int);
	void	(*old_sigquit)(int);

	if (cmd->argv && is_builtin(cmd->argv->value))
	{
		if (ft_strcmp(cmd->argv->value, "exit") == 0)
			return (exec_exit(shell, cmd->argv));
		return (exec_builtin(env_list, cmd, status));
	}
	old_sigint = signal(SIGINT, SIG_IGN);
	old_sigquit = signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid < 0)
		return (signal(SIGINT, old_sigint), signal(SIGQUIT, old_sigquit),
			exec_error(status, "fork"));
	if (pid == 0)
		ft_execve(env_list, cmd);
	waitpid(pid, &wstatus, 0);
	signal(SIGINT, old_sigint);
	signal(SIGQUIT, old_sigquit);
	handle_status(wstatus, status);
}

void	executor(t_shell *shell, t_node *ast, t_env *env_list, int *status)
{
	if (!ast)
		return ;
	if (ast->type == NODE_PIPE && g_status == 0)
		exec_pipe(shell, ast, status);
	else if ((ast->type == NODE_AND_IF || ast->type == NODE_OR_IF)
		&& g_status == 0)
	{
		executor(shell, ast->lhs, env_list, status);
		if (ast->type == NODE_AND_IF && *status == 0 && g_status == 0)
		{
			expander(ast->rhs, env_list, status);
			executor(shell, ast->rhs, env_list, status);
		}
		else if (ast->type == NODE_OR_IF && *status != 0 && g_status == 0)
		{
			expander(ast->rhs, env_list, status);
			executor(shell, ast->rhs, env_list, status);
		}
	}
	else if (ast->type == NODE_CMD && g_status == 0)
		exec_cmd(shell, env_list, ast->cmd, status);
}
