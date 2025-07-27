/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 22:41:43 by hkasamat          #+#    #+#             */
/*   Updated: 2025/07/27 14:40:41 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_synerr(TokenType expected)
{
	char	*token;

	if (expected == TOKEN_END || !expected)
		write(2, "minishell: syntax error: unexpected end of file\n", 48);
	else if (expected == TOKEN_NEWLINE)
		write(2, "minishell: syntax error: unexpected token `newline'\n", 52);
	else
	{
		if (expected == TOKEN_CLOSE_PAREN)
			token = ")";
		else if (expected == TOKEN_PIPE)
			token = "|";
		else if (expected == TOKEN_AND_IF)
			token = "&&";
		else if (expected == TOKEN_OR_IF)
			token = "||";
		write(2, "minishell: syntax error near unexpected token `", 47);
		write(2, token, ft_strlen(token));
		write(2, "'\n", 2);
	}
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_tokens(cmd->argv);
	free_fds(cmd->fds);
	free_fds(cmd->heredoc_delimiter);
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

int	token_cmd(t_token *tokens)
{
	if (!tokens)
		return (0);
	if (tokens->type == TOKEN_WORD || tokens->type == TOKEN_REDIR_IN
		|| tokens->type == TOKEN_REDIR_OUT || tokens->type == TOKEN_HEREDOC
		|| tokens->type == TOKEN_APPEND)
		return (1);
	return (0);
}

void	append_fd(t_fd **list, t_fd *new_fd)
{
	t_fd	*current;

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

t_node	*init_node(void)
{
	t_node	*node;

	node = (t_node *)malloc(sizeof(t_node));
	if (!node)
		return (perror("malloc"), NULL);
	node->type = NODE_INIT;
	node->lhs = NULL;
	node->rhs = NULL;
	node->cmd = NULL;
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
	cmd->heredoc_delimiter = NULL;
	return (cmd);
}

t_node	*parse_condition(t_token **tokens)
{
	t_node	*node;

	if ((*tokens)->type != TOKEN_OPEN_PAREN || !token_cmd(*tokens))
		return (NULL);
	node = init_node();
	if (!node)
		return (perror("malloc"), NULL);
	while (node && *tokens && (*tokens)->type != TOKEN_AND_IF
		&& (*tokens)->type != TOKEN_OR_IF)
	{
		if ((*tokens)->type == TOKEN_OPEN_PAREN)
			node = add_paren(tokens, node);
		else if ((*tokens)->type == TOKEN_PIPE)
			node = add_pipe(tokens, node);
		else if (token_cmd(*tokens))
			node = add_cmd(tokens, node);
	}
	return (node);
}

t_node	*parse_pipe(t_token **tokens)
{
	t_node	*node;

	node = init_node();
	if (!node)
		return (perror("malloc"), NULL);
	if (token_cmd(*tokens))
		return (add_cmd(tokens, node));
	else if ((*tokens)->type == TOKEN_OPEN_PAREN)
		return (add_paren(tokens, node));
	else
		return (NULL);
}

int	add_fd(t_cmd *cmd, t_token **tokens)
{
	t_fd	*fd;

	fd = (t_fd *)malloc(sizeof(t_fd));
	if (!fd)
		return (perror("malloc"), 0);
	fd->type = (*tokens)->type;
	fd->next = NULL;
	fd->fd = -1;
	(*tokens) = (*tokens)->next;
	if (!(*tokens))
		return (print_synerr(TOKEN_NEWLINE), free_fds(fd), 0);
	else if ((*tokens)->type != TOKEN_WORD)
		return (print_synerr((*tokens)->type), free_fds(fd), 0);
	fd->value = (*tokens)->value;
	if (fd->type == TOKEN_HEREDOC)
		append_fd(&cmd->heredoc_delimiter, fd);
	append_fd(&cmd->fds, fd);
	(*tokens) = (*tokens)->next;
	return (1);
}

int	add_argv(t_token *argv, t_token **tokens)
{
	t_token	*arg;

	arg = (t_token *)malloc(sizeof(t_token));
	if (!arg)
		return (perror("malloc"), 0);
	arg->type = (*tokens)->type;
	arg->value = (*tokens)->value;
	arg->next = NULL;
	(*tokens) = (*tokens)->next;
	append_token(&argv, arg);
	return (1);
}

t_cmd	*parse_cmd(t_token **tokens)
{
	t_cmd	*cmd;

	cmd = init_cmd();
	if (!cmd)
		return (perror("malloc"), NULL);
	while (*tokens && token_cmd(*tokens))
	{
		if ((*tokens)->type == TOKEN_WORD)
		{
			if (!add_argv(cmd->argv, tokens))
				return (free_cmd(cmd), NULL);
		}
		else if ((*tokens)->type == TOKEN_REDIR_IN
			|| (*tokens)->type == TOKEN_REDIR_OUT
			|| (*tokens)->type == TOKEN_HEREDOC
			|| (*tokens)->type == TOKEN_APPEND)
		{
			if (!add_fd(cmd, tokens))
				return (free_cmd(cmd), NULL);
		}
		else
			break ;
	}
	return (cmd);
}

t_node	*add_condition(t_token **tokens, t_node *node)
{
	t_node	*new_root;

	if (node->type == NODE_INIT)
		return (print_synerr((*tokens)->type), free_node(node), NULL);
	else if (!(*tokens)->next)
		return (print_synerr(TOKEN_END), free_node(node), NULL);
	else if (!(token_cmd((*tokens)->next)
			|| (*tokens)->next->type == TOKEN_OPEN_PAREN))
		return (print_synerr((*tokens)->next->type), free_node(node), NULL);
	new_root = init_node();
	if (!new_root)
		return (perror("malloc"), NULL);
	new_root->lhs = node;
	if ((*tokens)->type == TOKEN_AND_IF)
		new_root->type = NODE_AND_IF;
	else if ((*tokens)->type == TOKEN_OR_IF)
		new_root->type = NODE_OR_IF;
	*tokens = (*tokens)->next;
	new_root->rhs = parse_condition(tokens);
	if (!new_root->rhs)
		return (free_node(new_root), NULL);
	return (new_root);
}

t_node	*add_paren(t_token **tokens, t_node *node)
{
	t_node	*new_root;

	if (!(*tokens)->next)
		return (print_synerr(TOKEN_END), free_node(node), NULL);
	else if (!token_cmd((*tokens)->next)
		&& (*tokens)->next->type != TOKEN_OPEN_PAREN)
		return (print_synerr((*tokens)->next->type), free_node(node), NULL);
	new_root = init_node();
	if (!new_root)
		return (perror("malloc"), NULL);
	*tokens = (*tokens)->next;
	while (*tokens && (*tokens)->type != TOKEN_CLOSE_PAREN)
	{
		if ((*tokens)->type == TOKEN_AND_IF || (*tokens)->type == TOKEN_OR_IF)
			new_root = add_condition(tokens, new_root);
		else if ((*tokens)->type == TOKEN_OPEN_PAREN)
			new_root = add_paren(tokens, new_root);
		else if ((*tokens)->type == TOKEN_PIPE)
			new_root = add_pipe(tokens, new_root);
		else if (token_cmd(*tokens))
			new_root = add_cmd(tokens, new_root);
	}
	if (!(*tokens))
		return (free_node(new_root), print_synerr(TOKEN_END), NULL);
	*tokens = (*tokens)->next;
	return (new_root);
}

t_node	*add_pipe(t_token **tokens, t_node *node)
{
	t_node	*new_root;

	if (node->type == NODE_INIT)
		return (print_synerr(TOKEN_PIPE), free_node(node), NULL);
	else if ((*tokens)->next == NULL)
		return (print_synerr(TOKEN_END), free_node(node), NULL);
	else if (!token_cmd((*tokens)->next)
		&& (*tokens)->next->type != TOKEN_OPEN_PAREN)
		return (print_synerr((*tokens)->next->type), free_node(node), NULL);
	new_root = init_node();
	if (!new_root)
		return (perror("malloc"), NULL);
	new_root->lhs = node;
	new_root->type = NODE_PIPE;
	*tokens = (*tokens)->next;
	new_root->rhs = parse_pipe(tokens);
	if (!new_root->rhs)
		return (free_node(new_root), NULL);
	return (new_root);
}

t_node	*add_cmd(t_token **tokens, t_node *node)
{
	t_node	*new_node;

	if (node->type == NODE_INIT)
		new_node = node;
	else
		new_node = init_node();
	if (!new_node)
		return (perror("malloc"), NULL);
	new_node->type = NODE_CMD;
	new_node->cmd = parse_cmd(tokens);
	if (!new_node->cmd)
		return (free_node(new_node), free_node(node), NULL);
	return (new_node);
}

t_node	*parse(t_token *tokens)
{
	t_node	*node;

	if (!tokens)
		return (NULL);
	node = init_node();
	if (!node)
		return (perror("malloc"), free_tokens(tokens), NULL);
	while (node && tokens)
	{
		if (tokens->type == TOKEN_AND_IF || tokens->type == TOKEN_OR_IF)
			node = add_condition(&tokens, node);
		else if (tokens->type == TOKEN_OPEN_PAREN)
			node = add_paren(&tokens, node);
		else if (tokens->type == TOKEN_PIPE)
			node = add_pipe(&tokens, node);
		else if (token_cmd(tokens))
			node = add_cmd(&tokens, node);
		else
			return (print_synerr(tokens->type), free_node(node), NULL);
	}
	return (node);
}
