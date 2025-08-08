/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils8.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 01:13:43 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 01:22:54 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_token_str(t_TokenType token)
{
	if (token == TOKEN_PIPE)
		return ("|");
	else if (token == TOKEN_REDIR_IN)
		return ("<");
	else if (token == TOKEN_REDIR_OUT)
		return (">");
	else if (token == TOKEN_HEREDOC)
		return ("<<");
	else if (token == TOKEN_APPEND)
		return (">>");
	else if (token == TOKEN_OPEN_PAREN)
		return ("(");
	else if (token == TOKEN_CLOSE_PAREN)
		return (")");
	else if (token == TOKEN_AND_IF)
		return ("&&");
	else if (token == TOKEN_OR_IF)
		return ("||");
	else if (token == TOKEN_WORD)
		return ("TOKEN_WORD");
	return (NULL);
}
