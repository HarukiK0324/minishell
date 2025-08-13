/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:37 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 16:24:12 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void read_heredoc(t_fd *heredoc_delimiter, int fd)
{
    char *line;
    
    while (1)
    {
        line = readline("> ");
        if (!line || g_status != 0)  // Check for EOF or signal
        {
            if (fd != -1)
                close(fd);  // Close fd if interrupted
            if (line)
                free(line);
            return;
        }
        
        if (ft_strcmp(line, heredoc_delimiter->value) == 0)
        {
            free(line);
            return;
        }
        else if (fd != -1)
        {
            write(fd, line, ft_strlen(line));
            write(fd, "\n", 1);
        }
        free(line);
    }
}

void parse_heredoc(t_fd *heredoc_delimiter, int fd_in, int fd_out)
{
    // Set up a custom signal handler that will close file descriptors
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;  // Use default handler
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    
    // Close read end in child
    close(fd_out);
    
    if (!heredoc_delimiter)
    {
        close(fd_in);  // Make sure to close fd_in if we return early
        return;
    }
    
    while (heredoc_delimiter->next)
    {
        read_heredoc(heredoc_delimiter, -1);
        if (g_status != 0)  // Check if interrupted by signal
        {
            close(fd_in);  // Close fd_in before exiting
            exit(EXIT_FAILURE);
        }
        heredoc_delimiter->fd = -1;
        heredoc_delimiter = heredoc_delimiter->next;
    }
    
    read_heredoc(heredoc_delimiter, fd_in);
    heredoc_delimiter->fd = fd_out;
    close(fd_in);  // Normal close at the end
}

int ft_heredoc(t_cmd *cmd)
{
    int     fd[2];
    int     wstatus;
    pid_t   pid;

    if (pipe(fd) == -1)
        return (perror("pipe"), -1);
    
    heredoc_signal_hold(cmd);
    
    pid = fork();
    if (pid < 0)
    {
        close(fd[0]);
        close(fd[1]);
        return (perror("fork"), -1);
    }
    
    if (pid == 0)
    {
        reset_heredoc_signal();  // Set up signal handling in child
        parse_heredoc(cmd->heredoc_delimiter, fd[1], fd[0]);
        exit(EXIT_SUCCESS);
    }
    
    close(fd[1]);  // Close write end in parent
    cmd->heredoc_fd = fd[0];
    
    waitpid(pid, &wstatus, 0);
    heredoc_signal_revert(cmd);
    
    if (WIFSIGNALED(wstatus))
    {
        int sig = WTERMSIG(wstatus);
        if (sig == SIGINT)
        {
            g_status = 2;
            write(STDOUT_FILENO, "\n", 1);
        }
        close(fd[0]);
        return -1;
    }
    
    return 0;
}

void	process_heredoc(t_cmd *cmd, int *status)
{
	if (!cmd->heredoc_delimiter || g_status != 0)
		return ;
	if (ft_heredoc(cmd) == -1)
	{
		*status = 130;
		return ;
	}
}

void	heredoc(t_node *ast, int *status)
{
	if (!ast || g_status != 0)
		return ;
	if ((ast->type == NODE_PIPE || ast->type == NODE_AND_IF
			|| ast->type == NODE_OR_IF) && g_status == 0)
	{
		heredoc(ast->lhs, status);
		heredoc(ast->rhs, status);
	}
	else if (ast->type == NODE_CMD && g_status == 0)
		process_heredoc(ast->cmd, status);
}
