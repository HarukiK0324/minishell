/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils8.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 01:13:43 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 02:23:45 by hkasamat         ###   ########.fr       */
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

t_env	*init_env(char **environ)
{
	t_env	*env_list;
	t_env	*new_node;
	int		i;

	env_list = NULL;
	i = 0;
	while (environ[i])
	{
		new_node = (t_env *)malloc(sizeof(t_env));
		if (!new_node)
			return (perror("malloc"), free_env(env_list), NULL);
		new_node->key = ft_strndup(environ[i], ft_strchar(environ[i], '='));
		new_node->value = ft_strdup(environ[i] + ft_strchar(environ[i], '=')
				+ 1);
		new_node->next = env_list;
		env_list = new_node;
		i++;
	}
	return (env_list);
}

void	ft_exit(t_env *env_list, int status)
{
	free_env(env_list);
	printf("exit\n");
	if (g_status != 0)
		status = 128 + g_status;
	exit(status);
}

void	init_g_status(int *status)
{
	*status = 128 + g_status;
	g_status = 0;
}
