/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:15 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 01:01:48 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_node	*parse_condition(t_token **tokens)
{
	t_node	*node;
	t_node	*new_root;

	node = parse_pipe(tokens);
	while (node && (*tokens) && ((*tokens)->type == TOKEN_AND_IF
			|| (*tokens)->type == TOKEN_OR_IF))
	{
		new_root = init_node();
		if (!new_root)
			return (perror("malloc"), free_node(node), NULL);
		if ((*tokens)->type == TOKEN_AND_IF)
			new_root->type = NODE_AND_IF;
		else if ((*tokens)->type == TOKEN_OR_IF)
			new_root->type = NODE_OR_IF;
		*tokens = (*tokens)->next;
		new_root->lhs = node;
		new_root->rhs = parse_pipe(tokens);
		if (!new_root->rhs)
			return (free_node(new_root), NULL);
		node = new_root;
	}
	return (node);
}

t_node	*parse_pipe(t_token **tokens)
{
	t_node	*node;
	t_node	*new_root;

	node = parse_cmd(tokens);
	while (node && (*tokens) && (*tokens)->type == TOKEN_PIPE)
	{
		new_root = init_node();
		if (!new_root)
			return (perror("malloc"), free_node(node), NULL);
		new_root->lhs = node;
		new_root->type = NODE_PIPE;
		*tokens = (*tokens)->next;
		new_root->rhs = parse_cmd(tokens);
		if (!new_root->rhs)
			return (free_node(new_root), NULL);
		node = new_root;
	}
	return (node);
}

t_node	*create_cmd(t_token **tokens)
{
	t_node	*node;
	t_cmd	*cmd;

	node = init_node();
	if (!node)
		return (NULL);
	cmd = init_cmd();
	if (!cmd)
		return (free_node(node), NULL);
	while (*tokens && token_cmd(*tokens))
	{
		if ((*tokens)->type == TOKEN_WORD)
		{
			if (!add_argv(&cmd->argv, tokens))
				return (free_cmd(cmd), NULL);
		}
		else
		{
			if (!add_fd(cmd, tokens))
				return (free_cmd(cmd), NULL);
		}
	}
	node->cmd = cmd;
	return (node);
}

t_node	*parse_cmd(t_token **tokens)
{
	t_node	*node;

	if (!*tokens)
		return (print_synerr(TOKEN_END), NULL);
	else if ((*tokens)->type == TOKEN_OPEN_PAREN)
	{
		*tokens = (*tokens)->next;
		node = parse_condition(tokens);
		if (!node)
			return (NULL);
		else if (!(*tokens))
			return (print_synerr((TOKEN_END)), free_node(node), NULL);
		else if ((*tokens)->type != TOKEN_CLOSE_PAREN)
			return (print_synerr(((*tokens)->type)), free_node(node), NULL);
		(*tokens) = (*tokens)->next;
		return (node);
	}
	else if (!token_cmd(*tokens))
		return (print_synerr((*tokens)->type), NULL);
	return (create_cmd(tokens));
}

t_node	*parse(t_token *tokens)
{
	t_node	*node;

	if (!tokens)
		return (NULL);
	node = parse_condition(&tokens);
	return (node);
}
