/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils7.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:33 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 00:57:34 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	err_msg(char *value, char *msg)
{
	write(STDERR_FILENO, "minishell: ", 11);
	write(STDERR_FILENO, value, ft_strlen(value));
	write(STDERR_FILENO, msg, ft_strlen(msg));
}

void	err_msg_errno(char *value, char *msg)
{
	write(STDERR_FILENO, "minishell: ", 11);
	write(STDERR_FILENO, value, ft_strlen(value));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, msg, ft_strlen(msg));
	write(STDERR_FILENO, "\n", 1);
}

void	ft_open_heredoc(t_cmd *cmd, int heredoc_count)
{
	if (heredoc_count == cmd->heredoc_count)
	{
		if (cmd->fd_in != 0 && cmd->fd_in > 0)
			close(cmd->fd_in);
		cmd->fd_in = cmd->heredoc_fd;
	}
}

void	ft_open_fd_in(t_cmd *cmd, t_fd *current)
{
	if (cmd->fd_in != 0 && cmd->fd_in > 0)
		close(cmd->fd_in);
	current->fd = open(current->value, O_RDONLY);
	if (current->fd < 0)
		err_msg_errno(current->value, strerror(errno));
	cmd->fd_in = current->fd;
}

void	ft_open_fd_out(t_cmd *cmd, t_fd *current)
{
	if (cmd->fd_out != 1 && cmd->fd_out > 0)
		close(cmd->fd_out);
	if (current->type == TOKEN_REDIR_OUT)
		current->fd = open(current->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (current->type == TOKEN_APPEND)
		current->fd = open(current->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (current->fd < 0)
		err_msg_errno(current->value, strerror(errno));
	cmd->fd_out = current->fd;
}
