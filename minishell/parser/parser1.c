#include "minishell.h"

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
