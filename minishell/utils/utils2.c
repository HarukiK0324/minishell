/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:24 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/11 22:15:27 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_synerr(t_TokenType expected)
{
	char	*token;

	if (expected == TOKEN_END || !expected)
		write(2, "minishell: syntax error: unexpected end of file\n", 48);
	else if (expected == TOKEN_NEWLINE)
		write(2, "minishell: syntax error: unexpected token `newline'\n", 52);
	else
	{
		token = get_token_str(expected);
		write(2, "minishell: syntax error near unexpected token `", 47);
		write(2, token, ft_strlen(token));
		write(2, "'\n", 2);
	}
}

void	append_fd(t_fd **list, t_fd *new_fd)
{
	t_fd	*current;

	if (!new_fd)
		return ;
	if (!*list)
		*list = new_fd;
	else
	{
		current = *list;
		while (current->next)
			current = current->next;
		current->next = new_fd;
		new_fd->next = NULL;
	}
}

t_fd	*dup_fd(t_fd *fd)
{
	t_fd	*new_fd;

	new_fd = (t_fd *)malloc(sizeof(t_fd));
	if (!new_fd)
		return (perror("malloc"), NULL);
	new_fd->type = fd->type;
	new_fd->fd = fd->fd;
	new_fd->value = ft_strdup(fd->value);
	new_fd->next = NULL;
	return (new_fd);
}

t_node	*init_node(void)
{
	t_node	*node;

	node = (t_node *)malloc(sizeof(t_node));
	if (!node)
		return (perror("malloc"), NULL);
	node->type = NODE_CMD;
	node->lhs = NULL;
	node->rhs = NULL;
	node->cmd = NULL;
	node->old_sigint = NULL;
	node->old_sigquit = NULL;
	return (node);
}

t_cmd	*init_cmd(void)
{
	t_cmd	*cmd;

	cmd = (t_cmd *)malloc(sizeof(t_cmd));
	if (!cmd)
		return (perror("malloc"), NULL);
	cmd->argv = NULL;
	cmd->fds = NULL;
	cmd->fd_in = 0;
	cmd->fd_out = 1;
	cmd->heredoc_count = 0;
	cmd->heredoc_fd = -1;
	cmd->heredoc_delimiter = NULL;
	cmd->old_sigint = NULL;
	cmd->old_sigquit = NULL;
	return (cmd);
}
