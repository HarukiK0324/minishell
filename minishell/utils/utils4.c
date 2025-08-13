/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils4.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:28 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 23:58:01 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	ft_strchar(const char *s, char c)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			return (i);
		i++;
	}
	return (i);
}

void	free_ast(t_node *ast)
{
	if (!ast)
		return ;
	if (ast->cmd)
		free_cmd(ast->cmd);
	if (ast->lhs)
		free_ast(ast->lhs);
	if (ast->rhs)
		free_ast(ast->rhs);
	free(ast);
}

int	is_char(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

int	is_numchar(char c)
{
	return ((c >= '0' && c <= '9') || is_char(c));
}

char	*to_str(size_t n)
{
	char	*str;
	size_t	len;
	size_t	temp;

	len = 0;
	temp = n;
	if (n == 0)
		return (ft_strdup("0"));
	while (temp > 0)
	{
		temp /= 10;
		len++;
	}
	str = (char *)malloc(len + 1);
	if (!str)
		return (perror("malloc"), NULL);
	str[len] = '\0';
	while (n > 0)
	{
		str[--len] = (n % 10) + '0';
		n /= 10;
	}
	return (str);
}
