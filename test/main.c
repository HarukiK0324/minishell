/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:42 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/11 23:01:45 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_status = 0;

void set_status(int *status, int value)
{
	*status = value;
}

void	minishell(char *input, int *status, t_env *env_list)
{
	t_token	*tokens;
	t_node	*ast;

	if (check_quote(input) == -1)
		return (set_status(status, 2), free(input), err_msg("Unmatched quotes", ": Syntax error\n"));
	tokens = tokenize(input);
	ast = parse(tokens);
	if (!ast)
		set_status(status, 2);
	else
	{
		if (expander(ast, env_list, status) == -1)
			*status = 2;
		else
			*status = 0;
		heredoc(ast, status);
		if (g_status == 0 && *status == 0)
			executor(ast, env_list, status);
	}
	free_tokens(tokens);
	free_ast(ast);
	add_history(input);
	free(input);
	g_status = 0;
}

int	main(int argc, char **argv, char **environ)
{
	char	*input;
	t_env	*env_list;
	int		status;

	(void)argc;
	(void)argv;
	status = 0;
	env_list = init_env(environ);
	if (!env_list)
		return (perror("init_env failed"), 1);
	while (1)
	{
		setup_signal_handlers();
		input = readline("minishell$ ");
		if (!input)
			break ;
		if (g_status != 0)
			init_g_status(&status);
		if (ft_strlen(input) > 0)
			minishell(input, &status, env_list);
	}
	ft_exit(env_list, status);
}
