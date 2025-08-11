/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:22 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 00:57:23 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	append_token(t_token **list, t_token *new_token)
{
	t_token	*current;

	if (!*list)
		*list = new_token;
	else
	{
		current = *list;
		while (current->next)
			current = current->next;
		current->next = new_token;
		new_token->next = NULL;
	}
}

void	free_tokens(t_token *head)
{
	t_token	*tmp;

	while (head)
	{
		tmp = head->next;
		if (head->value)
			free(head->value);
		free(head);
		head = tmp;
	}
}

void	free_fds(t_fd *head)
{
	t_fd	*tmp;

	while (head)
	{
		tmp = head->next;
		if (head->value)
			free(head->value);
		free(head);
		head = tmp;
	}
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_tokens(cmd->argv);
	free_fds(cmd->fds);
	free(cmd);
}

void	free_node(t_node *node)
{
	if (!node)
		return ;
	if (node->cmd)
		free_cmd(node->cmd);
	if (node->lhs)
		free_node(node->lhs);
	if (node->rhs)
		free_node(node->rhs);
	free(node);
}
