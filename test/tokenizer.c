/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:50 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/11 22:12:22 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*init_token(t_TokenType type, const char *value, size_t value_len)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (write(STDERR_FILENO, "minishell: token creation\n", 26), NULL);
	token->type = type;
	token->value = ft_strndup(value, value_len);
	if (!token->value)
		return (free(token), perror("strndup"), NULL);
	token->next = NULL;
	return (token);
}

t_TokenType	get_meta_type(const char *s)
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
	return (TOKEN_ERROR);
}

size_t	add_word(const char *input, t_token **list)
{
	t_token	*token;
	size_t	len;
	char	quote;

	len = 0;
	while (input[len] && !ft_isblank(input[len]) && !ismetachar(input[len]))
	{
		if (input[len] == '"' || input[len] == '\'')
		{
			quote = input[len];
			len++;
			while (input[len] && input[len] != quote)
				len++;
			if (input[len] == '\0')
				return (write(STDERR_FILENO, "minishell: unclosed quote\n", 26),
					0);
		}
		len++;
	}
	token = init_token(TOKEN_WORD, input, len);
	if (token == NULL)
		return (0);
	append_token(list, token);
	return (len);
}

size_t	add_metachar(const char *input, t_token **list)
{
	t_TokenType	type;
	t_token		*token;
	size_t		len;

	type = get_meta_type(input);
	if (type == TOKEN_ERROR)
		return (write(STDERR_FILENO, "minishell: unknown token\n", 25), 0);
	if (type == TOKEN_HEREDOC || type == TOKEN_APPEND || type == TOKEN_AND_IF
		|| type == TOKEN_OR_IF)
		len = 2;
	else
		len = 1;
	token = init_token(type, input, len);
	if (!token)
		return (0);
	append_token(list, token);
	return (len);
}

t_token	*tokenize(const char *input)
{
	t_token	*tokens;
	size_t	len;

	tokens = NULL;
	while (*input)
	{
		while (*input && ft_isblank(*input))
			input++;
		if (!*input)
			break ;
		if (ismetachar(*input))
			len = add_metachar(input, &tokens);
		else
			len = add_word(input, &tokens);
		if (len <= 0)
			return (free_tokens(tokens), NULL);
		input += len;
	}
	return (tokens);
}
