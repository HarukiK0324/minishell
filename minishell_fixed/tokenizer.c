/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 22:41:53 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/02 14:36:25 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isblank(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

int	ismetachar(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '(' || c == ')' || c == ';'
		|| c == '&' || c == '`' || c == '\\');
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	while (n-- && *s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	if (n == (size_t)-1)
		return (0);
	return (*(const unsigned char *)s1 - *(const unsigned char *)s2);
}

char	*ft_strdup(const char *s)
{
	char	*str;
	size_t	i;

	if(!s)
		return (NULL);
	i = 0;
	str = (char *)malloc(ft_strlen(s) + 1);
	if (!str)
		return (perror("malloc"), NULL);
	while (s[i] != '\0')
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*str;
	size_t	i;

	i = 0;
	str = (char *)malloc(n + 1);
	if (!str)
		return (perror("malloc"), NULL);
	while (i < n && s[i] != '\0')
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

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
					-1);
		}
		len++;
	}
	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (write(STDERR_FILENO, "minishell: token creation\n", 26), -1);
	token->type = TOKEN_WORD;
	token->next = NULL;
	token->value = ft_strndup(input, len);
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
		return (write(STDERR_FILENO, "minishell: unknown token\n", 25), -1);
	if (type == TOKEN_HEREDOC || type == TOKEN_APPEND || type == TOKEN_AND_IF
		|| type == TOKEN_OR_IF)
		len = 2;
	else
		len = 1;
	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (write(STDERR_FILENO, "minishell: token creation\n", 26), -1);
	token->type = type;
	token->next = NULL;
	token->value = ft_strndup(input, len);
	append_token(list, token);
	return (len);
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
