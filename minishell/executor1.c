#include "minishell.h"

int	is_builtin(char *cmd)
{
	if (ft_strcmp(cmd, "cd") == 0 || ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "exit") == 0 || ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0)
		return (1);
	return (0);
}

void	exec_builtin(t_env *env_list, t_cmd *cmd, int *status)
{
	int	original_stdin;
	int	original_stdout;

	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	if (original_stdin == -1 || original_stdout == -1)
	{
		perror("dup");
		*status = 1;
		return ;
	}
	if (process_redirections(cmd) == -1)
	{
		*status = 1;
		if (cmd->fd_in != 0)
			close(cmd->fd_in);
		if (cmd->fd_out != 1)
			close(cmd->fd_out);
		close(original_stdin);
		close(original_stdout);
		return ;
	}
	if (cmd->fd_in != 0)
		dup2(cmd->fd_in, STDIN_FILENO);
	if (cmd->fd_out != 1)
		dup2(cmd->fd_out, STDOUT_FILENO);
	if (ft_strcmp(cmd->argv->value, "cd") == 0)
		*status = exec_cd(cmd->argv, env_list);
	else if (ft_strcmp(cmd->argv->value, "echo") == 0)
		*status = exec_echo(cmd->argv);
	else if (ft_strcmp(cmd->argv->value, "exit") == 0)
		*status = exec_exit(cmd->argv);
	else if (ft_strcmp(cmd->argv->value, "export") == 0)
		*status = exec_export(cmd->argv, env_list);
	else if (ft_strcmp(cmd->argv->value, "pwd") == 0)
		*status = exec_pwd();
	else if (ft_strcmp(cmd->argv->value, "unset") == 0)
		*status = exec_unset(cmd->argv, env_list);
	else if (ft_strcmp(cmd->argv->value, "env") == 0)
		*status = exec_env(env_list);
	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);
	if (cmd->fd_in != 0)
		close(cmd->fd_in);
	if (cmd->fd_out != 1)
		close(cmd->fd_out);
}

int	process_redirections(t_cmd *cmd)
{
	int		heredoc_count;
	int		status;
	t_fd	*current;

	status = 0;
	current = cmd->fds;
	if (current == NULL)
		return (0);
	heredoc_count = 0;
	while (current)
	{
		if (current->type == TOKEN_REDIR_IN)
			ft_open_fd_in(cmd, current);
		else if (current->type == TOKEN_HEREDOC)
			ft_open_heredoc(cmd, ++heredoc_count);
		else if (current->type == TOKEN_REDIR_OUT
			|| current->type == TOKEN_APPEND)
			ft_open_fd_out(cmd, current);
		if (cmd->fd_in == -1 || cmd->fd_out == -1)
			status = -1;
		current = current->next;
	}
	return (status);
}

void	ft_execve(t_env *env_list, t_cmd *cmd)
{
	char	*path;
	char	**environ;
	char	**argv;

	reset_default_signal();
	if (process_redirections(cmd) == -1)
		exit(EXIT_FAILURE);
	if (cmd->fd_in != 0)
	{
		if (dup2(cmd->fd_in, STDIN_FILENO) == -1)
			return (perror("dup2 input redirection failed"),
				exit(EXIT_FAILURE));
		close(cmd->fd_in);
	}
	if (cmd->fd_out != 1)
	{
		if (dup2(cmd->fd_out, STDOUT_FILENO) == -1)
			return (perror("dup2 output redirection failed"),
				exit(EXIT_FAILURE));
		close(cmd->fd_out);
	}
	if (!cmd->argv || !cmd->argv->value || ft_strlen(cmd->argv->value) == 0)
		exit(0);
	path = get_path(cmd->argv->value, env_list);
	if (!path || access(path, F_OK) == -1)
		return (err_msg(cmd->argv->value, ": command not found\n"), exit(127));
	argv = to_list(cmd->argv);
	environ = env_to_environ(env_list);
	if (g_status != 0)
		return (free(path), free_str_list(argv), free_str_list(environ),
			exit(128 + g_status));
	if (path && argv && environ)
		execve(path, argv, environ);
	if (path)
		free(path);
	free_str_list(argv);
	free_str_list(environ);
	if (errno == ENOENT)
		return (err_msg(cmd->argv->value, ": command not found\n"), exit(127));
	err_msg_errno(cmd->argv->value, strerror(errno));
	if (errno == EACCES)
		exit(126);
	exit(EXIT_FAILURE);
}
