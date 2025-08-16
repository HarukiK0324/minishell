/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:05 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/16 13:42:56 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_builtin(t_env *env_list, t_cmd *cmd, int *status)
{
	int	original_stdin;
	int	original_stdout;

	original_stdin = dup(STDIN_FILENO);
	if (original_stdin == -1)
		return (exec_error(status, "dup"));
	original_stdout = dup(STDOUT_FILENO);
	if (original_stdout == -1)
		return (close(original_stdin), exec_error(status, "dup"));
	if (process_redirections(cmd) == -1)
		return (handle_builtin_fd(cmd, status, original_stdin,
				original_stdout));
	if (cmd->fd_in != 0)
		dup2(cmd->fd_in, STDIN_FILENO);
	if (cmd->fd_out != 1)
		dup2(cmd->fd_out, STDOUT_FILENO);
	run_builtin(env_list, cmd, status);
	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);
	if (cmd->fd_in != 0)
		close(cmd->fd_in);
	if (cmd->fd_out != 1)
		close(cmd->fd_out);
}

int	process_redirections(t_cmd *cmd)
{
	int		heredoc_count;
	int		status;
	t_fd	*current;

	status = 0;
	current = cmd->fds;
	if (current == NULL)
		return (0);
	heredoc_count = 0;
	while (current && status == 0)
	{
		if (current->type == TOKEN_REDIR_IN)
			ft_open_fd_in(cmd, current);
		else if (current->type == TOKEN_HEREDOC)
			ft_open_heredoc(cmd, ++heredoc_count);
		else if (current->type == TOKEN_REDIR_OUT
			|| current->type == TOKEN_APPEND)
			ft_open_fd_out(cmd, current);
		if (cmd->fd_in == -1 || cmd->fd_out == -1)
			status = -1;
		current = current->next;
	}
	return (status);
}

void	execve_dup(t_cmd *cmd)
{
	if (cmd->fd_in != 0)
	{
		if (dup2(cmd->fd_in, STDIN_FILENO) == -1)
		{
			perror("dup2 input redirection failed");
			if (cmd->fd_out != 1)
				close(cmd->fd_out);
			close(cmd->fd_in);
			exit(EXIT_FAILURE);
		}
		close(cmd->fd_in);
	}
	if (cmd->fd_out != 1)
	{
		if (dup2(cmd->fd_out, STDOUT_FILENO) == -1)
		{
			perror("dup2 output redirection failed");
			if (cmd->fd_in != 0)
				close(cmd->fd_in);
			close(cmd->fd_out);
			exit(EXIT_FAILURE);
		}
		close(cmd->fd_out);
	}
}

void	free_all_arg(char *path, char **argv, char **environ)
{
	if (path)
		free(path);
	free_str_list(argv);
	free_str_list(environ);
}

void	ft_execve(t_env *env_list, t_cmd *cmd)
{
	char	*path;
	char	**environ;
	char	**argv;

	reset_default_signal();
	if (process_redirections(cmd) == -1)
		exit(EXIT_FAILURE);
	execve_dup(cmd);
	if (!cmd->argv || !cmd->argv->value || ft_strlen(cmd->argv->value) == 0)
		exit(0);
	path = get_path(cmd->argv->value, env_list);
	if (!path || access(path, F_OK) == -1)
		return (err_msg(cmd->argv->value, ": command not found\n"), exit(127));
	argv = to_list(cmd->argv);
	environ = env_to_environ(env_list);
	if (g_status != 0)
		return (free(path), free_str_list(argv), free_str_list(environ),
			exit(128 + g_status));
	if (path && argv && environ)
		execve(path, argv, environ);
	free_all_arg(path, argv, environ);
	err_msg_errno(cmd->argv->value, strerror(errno));
	if (errno == EACCES)
		exit(126);
	exit(EXIT_FAILURE);
}
