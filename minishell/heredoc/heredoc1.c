/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 03:39:19 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 20:22:30 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	heredoc_signal_hold(t_cmd *cmd)
{
	cmd->old_sigint = signal(SIGINT, SIG_IGN);
	cmd->old_sigquit = signal(SIGQUIT, SIG_IGN);
}

void	heredoc_signal_revert(t_cmd *cmd)
{
	signal(SIGINT, cmd->old_sigint);
	signal(SIGQUIT, cmd->old_sigquit);
}
