/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:15 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 20:22:33 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_node	*parse_condition(t_token **tokens, int *status)
{
	t_node	*node;
	t_node	*new_root;

	node = parse_pipe(tokens, status);
	while (node && (*tokens) && ((*tokens)->type == TOKEN_AND_IF
			|| (*tokens)->type == TOKEN_OR_IF))
	{
		new_root = init_node();
		if (!new_root)
			return (set_status(status, 1), free_node(node), NULL);
		if ((*tokens)->type == TOKEN_AND_IF)
			new_root->type = NODE_AND_IF;
		else if ((*tokens)->type == TOKEN_OR_IF)
			new_root->type = NODE_OR_IF;
		*tokens = (*tokens)->next;
		new_root->lhs = node;
		new_root->rhs = parse_pipe(tokens, status);
		if (!new_root->rhs)
			return (free_node(new_root), NULL);
		node = new_root;
	}
	return (node);
}

t_node	*parse_pipe(t_token **tokens, int *status)
{
	t_node	*node;
	t_node	*new_root;

	node = parse_cmd(tokens, status);
	while (node && (*tokens) && (*tokens)->type == TOKEN_PIPE)
	{
		new_root = init_node();
		if (!new_root)
			return (set_status(status, 1), free_node(node), NULL);
		new_root->lhs = node;
		new_root->type = NODE_PIPE;
		*tokens = (*tokens)->next;
		new_root->rhs = parse_cmd(tokens, status);
		if (!new_root->rhs)
			return (free_node(new_root), NULL);
		node = new_root;
	}
	return (node);
}

t_node	*create_cmd(t_token **tokens, int *status)
{
	t_node	*node;
	t_cmd	*cmd;

	node = init_node();
	if (!node)
		return (set_status(status, 1), NULL);
	cmd = init_cmd();
	if (!cmd)
		return (set_status(status, 1), free_node(node), NULL);
	while (*tokens && token_cmd(*tokens))
	{
		if ((*tokens)->type == TOKEN_WORD)
		{
			if (!add_argv(&cmd->argv, tokens))
				return (set_status(status, 1), free_cmd(cmd), NULL);
		}
		else
		{
			if (!add_fd(cmd, tokens, status))
				return (free_cmd(cmd), NULL);
		}
	}
	node->cmd = cmd;
	return (node);
}

t_node	*parse_cmd(t_token **tokens, int *status)
{
	t_node	*node;

	if (!*tokens)
		return (set_status(status, 2), print_synerr(TOKEN_END), NULL);
	else if ((*tokens)->type == TOKEN_OPEN_PAREN)
	{
		*tokens = (*tokens)->next;
		node = parse_condition(tokens, status);
		if (!node)
			return (NULL);
		else if (!(*tokens))
			return (set_status(status, 2), print_synerr((TOKEN_END)),
				free_node(node), NULL);
		else if ((*tokens)->type != TOKEN_CLOSE_PAREN)
			return (set_status(status, 2), print_synerr(((*tokens)->type)),
				free_node(node), NULL);
		(*tokens) = (*tokens)->next;
		return (node);
	}
	else if (!token_cmd(*tokens))
		return (set_status(status, 2), print_synerr((*tokens)->type), NULL);
	return (create_cmd(tokens, status));
}

t_node	*parse(t_token *tokens, int *status)
{
	t_node	*node;

	if (!tokens)
		return (NULL);
	node = parse_condition(&tokens, status);
	return (node);
}
