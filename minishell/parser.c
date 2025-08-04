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

	if(!new_fd)
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
	t_node *new_root;

	node = parse_pipe(tokens);
	while(node && (*tokens) && ((*tokens)->type == TOKEN_AND_IF || (*tokens)->type == TOKEN_OR_IF))
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
		if(!new_root->rhs)
			return (free_node(new_root), NULL);
		node = new_root;
	}
	return (node);
}

t_node	*parse_pipe(t_token **tokens)
{
	t_node	*node;
	t_node *new_root;

	node = parse_cmd(tokens);
	while(node && (*tokens) && (*tokens)->type == TOKEN_PIPE)
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
	fd->value = ft_strdup((*tokens)->value);
	if (fd->type == TOKEN_HEREDOC)
	{
		cmd->heredoc_count++;
		append_fd(&cmd->heredoc_delimiter, dup_fd(fd));
	}
	append_fd(&cmd->fds, fd);
	(*tokens) = (*tokens)->next;
	return (1);
}

int	add_argv(t_token **argv, t_token **tokens)
{
	t_token	*arg;

	arg = (t_token *)malloc(sizeof(t_token));
	if (!arg)
		return (perror("malloc"), 0);
	arg->type = (*tokens)->type;
	arg->value = ft_strdup((*tokens)->value);
	arg->next = NULL;
	(*tokens) = (*tokens)->next;
	append_token(argv, arg);
	return (1);
}

t_node	*parse_cmd(t_token **tokens)
{
	t_node	*node;
	t_cmd	*cmd;

	if (!*tokens)
		return (print_synerr(TOKEN_END), NULL);
	else if((*tokens)->type == TOKEN_OPEN_PAREN)
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
	else if(!token_cmd(*tokens))
		return (print_synerr((*tokens)->type), NULL);
	node = init_node();
	cmd = init_cmd();
	if (!cmd || !node)
		return (perror("malloc"), free_cmd(cmd), free_node(node), NULL);
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

t_node	*parse(t_token *tokens)
{
	t_node	*node;

	if (!tokens)
		return (NULL);
	node = parse_condition(&tokens);
	return (node);
}
