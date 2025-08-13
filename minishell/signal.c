/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:48 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 22:13:11 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	setup_signal_handlers(void)
{
	struct sigaction	sa_int;

	sa_int.sa_handler = handle_interactive_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	handle_interactive_sigint(int sig)
{
	(void)sig;
	g_status = 2;
	rl_on_new_line();
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_redisplay();
}

void	reset_default_signal(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	heredoc_int_handler(int sig)
{
	int	devnull;

	(void)sig;
	write(1, "\n", 1);
	g_status = 2;
	rl_replace_line("", 0);
	devnull = open("/dev/null", O_RDONLY);
	if (devnull >= 0)
		dup2(devnull, STDIN_FILENO);
}

void	reset_heredoc_signal(void)
{
	struct sigaction	sa_int;

	sa_int.sa_handler = heredoc_int_handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	signal(SIGQUIT, SIG_IGN);
}
