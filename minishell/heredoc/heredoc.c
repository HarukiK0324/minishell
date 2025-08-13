/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:37 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 19:42:44 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	read_heredoc(t_fd *heredoc_delimiter, int fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			return ;
		if (ft_strcmp(line, heredoc_delimiter->value) == 0)
			return (free(line));
		else if (fd != -1)
		{
			write(fd, line, ft_strlen(line));
			write(fd, "\n", 1);
		}
		free(line);
	}
}

void	parse_heredoc(t_fd *heredoc_delimiter, int fd_in, int fd_out)
{
	reset_heredoc_signal();
	close(fd_out);
	if (!heredoc_delimiter)
		return ;
	while (heredoc_delimiter->next && g_status == 0)
	{
		read_heredoc(heredoc_delimiter, -1);
		heredoc_delimiter->fd = -1;
		heredoc_delimiter = heredoc_delimiter->next;
	}
	if (g_status == 0)
	{
		read_heredoc(heredoc_delimiter, fd_in);
		heredoc_delimiter->fd = fd_out;
	}
	close(fd_in);
}

int	ft_heredoc(t_cmd *cmd, int *run_status)
{
	int		fd[2];
	int		wstatus;
	pid_t	pid;

	if (pipe(fd) == -1)
		return (set_status(run_status, 1), perror("pipe"), -1);
	heredoc_signal_hold(cmd);
	pid = fork();
	if (pid < 0)
		return (set_status(run_status, 1), perror("fork"), -1);
	if (pid == 0)
	{
		parse_heredoc(cmd->heredoc_delimiter, fd[1], fd[0]);
		exit(EXIT_SUCCESS);
	}
	close(fd[1]);
	cmd->heredoc_fd = fd[0];
	waitpid(pid, &wstatus, 0);
	heredoc_signal_revert(cmd);
	if (WTERMSIG(wstatus) == SIGINT)
	{
		g_status = 2;
		return (write(STDOUT_FILENO, "\n", 1), close(fd[0]), -1);
	}
	return (0);
}

void	process_heredoc(t_cmd *cmd, int *run_status)
{
	if (!cmd->heredoc_delimiter || g_status != 0)
		return ;
	ft_heredoc(cmd, run_status);
}

void	heredoc(t_node *ast, int *run_status)
{
	if (!ast || g_status != 0 || run_status != 0)
		return ;
	if ((ast->type == NODE_PIPE || ast->type == NODE_AND_IF
			|| ast->type == NODE_OR_IF) && g_status == 0 && run_status == 0)
	{
		heredoc(ast->lhs, run_status);
		heredoc(ast->rhs, run_status);
	}
	else if (ast->type == NODE_CMD && g_status == 0 && run_status == 0)
		process_heredoc(ast->cmd, run_status);
}
