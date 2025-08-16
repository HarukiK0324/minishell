/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 03:40:37 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/16 13:33:00 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	pipe_cleanup(int fd[], t_node *ast, int *status)
{
	close(fd[0]);
	close(fd[1]);
	signal_pipe_revert(ast);
	exec_error(status, "fork");
}

void	signal_pipe_hold(t_node *ast)
{
	ast->old_sigint = signal(SIGINT, SIG_IGN);
	ast->old_sigquit = signal(SIGQUIT, SIG_IGN);
}

void	signal_pipe_revert(t_node *ast)
{
	signal(SIGINT, ast->old_sigint);
	signal(SIGQUIT, ast->old_sigquit);
}
