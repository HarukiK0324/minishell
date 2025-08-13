/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:42 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 23:42:40 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_status = 0;

int	init_shell(t_shell **shell, char **environ)
{
	*shell = (t_shell *)malloc(sizeof(t_shell));
	if (!*shell)
		return (perror("malloc"), -1);
	(*shell)->status = malloc(sizeof(int));
	if (!(*shell)->status)
		return (free(*shell), perror("malloc"), -1);
	(*shell)->run_status = malloc(sizeof(int));
	if (!(*shell)->run_status)
		return (free((*shell)->status), free(*shell), perror("malloc"), -1);
	*((*shell)->status) = 0;
	*((*shell)->run_status) = 0;
	(*shell)->input = NULL;
	(*shell)->env_list = NULL;
	(*shell)->ast = NULL;
	(*shell)->tokens = NULL;
	if (init_env(&((*shell)->env_list), environ) == -1)
		return (-1);
	return (0);
}

void	set_status(int *status, int value)
{
	*status = value;
}

void	clean_up(t_shell *shell)
{
	free_tokens(shell->tokens);
	shell->tokens = NULL;
	free_ast(shell->ast);
	shell->ast = NULL;
	if (shell->input && ft_strlen(shell->input) > 0)
		add_history(shell->input);
	if (shell->input)
		free(shell->input);
	shell->input = NULL;
	init_g_status(shell->status);
	g_status = 0;
	*(shell->run_status) = 0;
}

void	minishell(t_shell *shell)
{
	shell->tokens = tokenize(shell->input, shell->run_status);
	if (*(shell->run_status) != 0)
		return (set_status(shell->status, *(shell->run_status)),
			clean_up(shell));
	shell->ast = parse(shell->tokens, shell->run_status);
	if (*(shell->run_status) != 0)
		return (set_status(shell->status, *(shell->run_status)),
			clean_up(shell));
	if (expander(shell->ast, shell->env_list, shell->status) == -1)
		return (set_status(shell->status, 1), clean_up(shell));
	heredoc(shell->ast, shell->run_status);
	if (g_status != 0)
		return (set_status(shell->status, 130), clean_up(shell));
	else if (*(shell->run_status) == 1)
		return (set_status(shell->status, 1), clean_up(shell));
	executor(shell, shell->ast, shell->env_list, shell->status);
	clean_up(shell);
}

int	main(int argc, char **argv, char **environ)
{
	t_shell	*shell;

	(void)argc;
	(void)argv;
	shell = NULL;
	if (init_shell(&shell, environ) == -1)
		return (1);
	while (1)
	{
		setup_signal_handlers();
		shell->input = readline("minishell$ ");
		init_g_status(shell->status);
		if (!shell->input)
			break ;
		if (check_quote(shell->input) == -1)
		{
			set_status(shell->status, 2);
			free(shell->input);
			write(STDERR_FILENO, "Syntax error", 12);
			continue ;
		}
		if (ft_strlen(shell->input) > 0)
			minishell(shell);
	}
	ft_exit(shell, shell->env_list, *shell->status);
}
