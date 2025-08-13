/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:48 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 16:24:25 by hkasamat         ###   ########.fr       */
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

void reset_heredoc_signal(void)
{
    struct sigaction sa;
    
    sa.sa_handler = SIG_DFL;  // Use default handler for SIGINT
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    
    signal(SIGQUIT, SIG_IGN);  // Ignore SIGQUIT
}