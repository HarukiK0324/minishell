/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:37 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/13 16:46:05 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	read_heredoc(t_fd *heredoc_delimiter, int fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		// Handle EOF or NULL
		if (!line)
			return;
		// Check for delimiter match
		if (ft_strcmp(line, heredoc_delimiter->value) == 0)
		{
			free(line);
			return;
		}
		// Write to file if valid fd
		if (fd != -1)
		{
			write(fd, line, ft_strlen(line));
			write(fd, "\n", 1);
		}
		free(line);
	}
}

// Generate a unique temporary filename
char	*generate_temp_filename(void)
{
	static int	counter = 0;
	char		*filename;
	char		*num_str;
	pid_t		pid;

	// Use both counter and process ID for uniqueness
	pid = getpid();
	num_str = to_str(pid * 10000 + counter++);
	if (!num_str)
		return (NULL);
	filename = (char *)malloc(ft_strlen("/tmp/heredoc_") + ft_strlen(num_str) + 1);
	if (!filename)
		return (free(num_str), NULL);
	filename[0] = '\0';
	ft_strcat(filename, "/tmp/heredoc_");
	ft_strcat(filename, num_str);
	free(num_str);
	return (filename);
}

void	parse_heredoc(t_fd *heredoc_delimiter, char *temp_file)
{
	int	fd;
	
	// Set up signal handling for heredoc
	reset_heredoc_signal();
	
	if (!heredoc_delimiter)
		return;
	
	// Open temp file with restrictive permissions (only user can read/write)
	fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
		return;
	
	// Process all but the last heredoc delimiter (these are just skipped)
	while (heredoc_delimiter->next)
	{
		read_heredoc(heredoc_delimiter, -1);
		heredoc_delimiter->fd = -1;
		heredoc_delimiter = heredoc_delimiter->next;
	}
	
	// Process the last heredoc delimiter (this one actually writes to the file)
	read_heredoc(heredoc_delimiter, fd);
	
	// Explicitly close fd - though OS would do this when process terminates
	close(fd);
}

int	ft_heredoc(t_cmd *cmd)
{
	int		wstatus;
	pid_t	pid;
	char	*temp_file;

	// Generate unique temp filename
	temp_file = generate_temp_filename();
	if (!temp_file)
		return (-1);
	
	// Set up signal handling for parent
	heredoc_signal_hold(cmd);
	
	pid = fork();
	if (pid < 0)
		return (free(temp_file), perror("fork"), -1);
	
	if (pid == 0)
	{
		// Child process: read heredoc and write to file
		parse_heredoc(cmd->heredoc_delimiter, temp_file);
		exit(EXIT_SUCCESS);
	}
	
	// Parent waits for child to finish
	waitpid(pid, &wstatus, 0);
	heredoc_signal_revert(cmd);
	
	// Handle child termination by signal
	if (WIFSIGNALED(wstatus))
	{
		if (WTERMSIG(wstatus) == SIGINT)
		{
			g_status = 2;
			write(STDOUT_FILENO, "\n", 1);
		}
		// Clean up temp file
		unlink(temp_file);
		free(temp_file);
		return (-1);
	}
	
	// Open the temp file for reading
	cmd->heredoc_fd = open(temp_file, O_RDONLY);
	
	// Delete the temp file but keep fd open
	// This ensures the file is removed from the filesystem
	// but can still be read from via the open file descriptor
	unlink(temp_file);
	free(temp_file);
	
	if (cmd->heredoc_fd == -1)
		return (-1);
	
	return (0);
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
