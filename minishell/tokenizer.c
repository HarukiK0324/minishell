#include "minishell.h"

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f'
		|| c == '\r');
}

int	ismetachar(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '(' || c == ')' || c == ';'
		|| c == '&' || c == '$' || c == '`' || c == '\\' || c == '=');
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*new;

	new = (char *)malloc(n + 1);
	if (!new)
		return (NULL);
	for (size_t i = 0; i < n; i++)
		new[i] = s[i];
	new[n] = '\0';
	return (new);
}

void	append_token(t_token **list, t_token *new_token)
{
	t_token	*cur;

	if (!*list)
		*list = new_token;
	else
	{
		cur = *list;
		while (cur->next)
			cur = cur->next;
		cur->next = new_token;
	}
}

TokenType	get_meta_type(const char *s)
{
	if (ft_strncmp(s, "<<", 2) == 0)
		return (TOKEN_HEREDOC);
	if (ft_strncmp(s, ">>", 2) == 0)
		return (TOKEN_APPEND);
	if (ft_strncmp(s, "&&", 2) == 0)
		return (TOKEN_AND_IF);
	if (ft_strncmp(s, "||", 2) == 0)
		return (TOKEN_OR_IF);
	if (ft_strncmp(s, "|", 1) == 0)
		return (TOKEN_PIPE);
	if (ft_strncmp(s, "<", 1) == 0)
		return (TOKEN_REDIR_IN);
	if (ft_strncmp(s, ">", 1) == 0)
		return (TOKEN_REDIR_OUT);
	if (ft_strncmp(s, "(", 1) == 0)
		return (TOKEN_OPEN_PAREN);
	if (ft_strncmp(s, ")", 1) == 0)
		return (TOKEN_CLOSE_PAREN);
	if (ft_strncmp(s, ";", 1) == 0)
		return (TOKEN_SEMICOLON);
	if (ft_strncmp(s, "$", 1) == 0)
		return (TOKEN_DOLLAR);
	return (TOKEN_WORD);
}

int	add_word(const char *input, t_token **list)
{
	size_t	len;
	t_token	*token;

	len = 0;
	while (input[len] && !ft_isspace(input[len]) && !ismetachar(input[len]))
		len++;
	if (len == 0)
		return (-1);
	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (-1);
	token->type = TOKEN_WORD;
	token->value = ft_strndup(input, len);
	token->next = NULL;
	append_token(list, token);
	return (len);
}

int	add_metachar(const char *input, t_token **list)
{
	TokenType	type;
	int			len;
	t_token		*token;

	type = get_meta_type(input);
	len = (type == TOKEN_HEREDOC || type == TOKEN_APPEND || type == TOKEN_AND_IF
			|| type == TOKEN_OR_IF) ? 2 : 1;
	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (-1);
	token->type = type;
	token->value = ft_strndup(input, len);
	token->next = NULL;
	append_token(list, token);
	return (len);
}

int	add_quote(const char *input, t_token **list, char quote)
{
	const char	*start = input + 1;
	size_t		len;
	t_token		*token;

	len = 0;
	while (start[len] && start[len] != quote)
		len++;
	if (start[len] != quote)
		return (-1);
	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (-1);
	token->type = (quote == '"') ? TOKEN_DOUBLE_QUOTE : TOKEN_SINGLE_QUOTE;
	token->value = ft_strndup(start, len);
	token->next = NULL;
	append_token(list, token);
	return (int)(len + 2);
}

void	free_tokens(t_token *head)
{
	t_token	*tmp;

	while (head)
	{
		tmp = head->next;
		free(head->value);
		free(head);
		head = tmp;
	}
}

t_token	*tokenize(const char *input)
{
	t_token	*tokens;
	int		consumed;

	tokens = NULL;
	while (*input)
	{
		while (*input && ft_isspace(*input))
			input++;
		if (!*input)
			break ;
		consumed = 0;
		if (*input == '\'' || *input == '"')
			consumed = add_quote(input, &tokens, *input);
		else if (ismetachar(*input))
			consumed = add_metachar(input, &tokens);
		else
			consumed = add_word(input, &tokens);
		if (consumed <= 0)
			return (free_tokens(tokens), NULL);
		input += consumed;
	}
	return (tokens);
}
