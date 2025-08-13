/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:37 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 16:25:49 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void read_heredoc(t_fd *heredoc_delimiter, int fd)
{
    char    *line;

    while (1)
    {
        line = readline("> ");
        if (!line)
            return (free(line));
        if (ft_strcmp(line, heredoc_delimiter->value) == 0)
            return (free(line));
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
    reset_heredoc_signal();
    
    // Immediately close the read end in the child as we only need to write
    close(fd_out);
    
    if (!heredoc_delimiter)
        return;
        
    while (heredoc_delimiter->next)
    {
        read_heredoc(heredoc_delimiter, -1);
        heredoc_delimiter->fd = -1;
        heredoc_delimiter = heredoc_delimiter->next;
    }
    
    read_heredoc(heredoc_delimiter, fd_in);
    heredoc_delimiter->fd = fd_out;
    close(fd_in);
}

int ft_heredoc(t_cmd *cmd)
{
    int     fd[2];
    int     wstatus;
    pid_t   pid;

    if (pipe(fd) == -1)
        return (perror("pipe"), -1);
    
    heredoc_signal_hold(cmd);
    
    // Create two separate pipes - one for heredoc content, one for signaling completion
    int signal_pipe[2];
    if (pipe(signal_pipe) == -1)
    {
        close(fd[0]);
        close(fd[1]);
        return (perror("pipe"), -1);
    }
    
    pid = fork();
    if (pid < 0)
    {
        close(fd[0]);
        close(fd[1]);
        close(signal_pipe[0]);
        close(signal_pipe[1]);
        return (perror("fork"), -1);
    }
    
    if (pid == 0)
    {
        // Child process
        close(signal_pipe[0]); // Close read end of signal pipe
        
        // Use dup2 to redirect stdin/stdout as needed for heredoc handling
        parse_heredoc(cmd->heredoc_delimiter, fd[1], fd[0]);
        
        // Write to signal pipe to indicate successful completion
        write(signal_pipe[1], "1", 1);
        close(signal_pipe[1]);
        
        exit(EXIT_SUCCESS);
    }
    
    // Parent process
    close(fd[1]);  // Close write end of heredoc pipe
    close(signal_pipe[1]); // Close write end of signal pipe
    
    cmd->heredoc_fd = fd[0];
    
    // Wait for child process to finish
    waitpid(pid, &wstatus, 0);
    heredoc_signal_revert(cmd);
    
    // Check if we received completion signal
    char buf[2] = {0};
    int signal_read = read(signal_pipe[0], buf, 1);
    close(signal_pipe[0]);
    
    if (WIFSIGNALED(wstatus))
    {
        // Child was terminated by a signal
        if (WTERMSIG(wstatus) == SIGINT)
        {
            g_status = 2;
            write(STDOUT_FILENO, "\n", 1);
        }
        close(fd[0]);
        return -1;
    }
    
    // If no completion signal was received but child exited normally,
    // it means the child exited without properly closing FDs
    if (signal_read <= 0 && WIFEXITED(wstatus))
    {
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
