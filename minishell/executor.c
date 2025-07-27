/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 22:40:58 by hkasamat          #+#    #+#             */
/*   Updated: 2025/07/27 17:49:02 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
}

void	free_str_list(char **list)
{
	size_t	i;

	if (!list)
		return ;
	i = 0;
	while (list[i])
		free(list[i++]);
	free(list);
}

char	*ft_access(char *path, char *cmd)
{
	char	*full_path;
	int		i;
	int		j;

	full_path = (char *)malloc(ft_strlen(path) + ft_strlen(cmd) + 2);
	if (!full_path)
		return (perror("malloc"), NULL);
	i = -1;
	while (path && path[++i] != '\0')
		full_path[i] = path[i];
	full_path[i++] = '/';
	j = 0;
	while (cmd && cmd[j] != '\0')
		full_path[i++] = cmd[j++];
	full_path[i] = '\0';
	if (access(full_path, F_OK) == -1)
		return (free(full_path), NULL);
	return (full_path);
}

char	*get_path_from_env(char *argv, t_env *env_list)
{
	char	**paths;
	int		i;
	char	*path;

	i = 0;
	while (ft_strcmp(env_list->key, "PATH") != 0)
		env_list = env_list->next;
	if (!env_list)
		return (write(STDERR_FILENO, "minishell: ", 11), write(STDERR_FILENO,
				argv, ft_strlen(argv)), write(STDERR_FILENO,
				": No such file or directory\n", 28), NULL);
	paths = ft_split(env_list->value, ':');
	path = NULL;
	while (paths && paths[i])
		path = ft_access(paths[i++], argv);
	i = 0;
	free_str_list(paths);
	if (path)
		return (path);
	return (write(STDERR_FILENO, "minishell: ", 11), write(STDERR_FILENO, argv,
			ft_strlen(argv)), write(STDERR_FILENO, ": command not found\n", 20),
		NULL);
}

char	*get_path(char *cmd, t_env *env_list)
{
	if (ft_strchar(cmd, '/') == ft_strlen(cmd))
		return (ft_strdup(cmd));
	return (get_path_from_env(cmd, env_list));
}

size_t	ft_token_size(t_token *tokens)
{
	size_t	size;

	size = 0;
	while (tokens)
	{
		size++;
		tokens = tokens->next;
	}
	return (size);
}

size_t	ft_env_size(t_env *env_list)
{
	size_t	size;

	size = 0;
	while (env_list)
	{
		size++;
		env_list = env_list->next;
	}
	return (size);
}

void	free_argv(char **argv, size_t i)
{
	size_t	index;

	index = 0;
	while (index < i)
	{
		free(argv[index]);
		index++;
	}
	free(argv);
}

char	**to_list(t_token *argv)
{
	char	**list;
	size_t	i;

	if (!argv)
		return (NULL);
	list = (char **)malloc(sizeof(char *) * (ft_token_size(argv) + 1));
	if (!list)
		return (perror("malloc"), NULL);
	i = 0;
	while (argv)
	{
		list[i] = ft_strdup(argv->value);
		if (!list[i])
			return (perror("strdup"), free_argv(list, i), NULL);
		argv = argv->next;
		i++;
	}
	list[i] = NULL;
	return (list);
}

char	*ft_env_join(t_env *env_list)
{
	char	*env;
	int		i;
	int		j;

	if (!env_list)
		return (NULL);
	env = (char *)malloc(ft_strlen(env_list->key) + ft_strlen(env_list->value)
			+ 2);
	if (!env)
		return (perror("malloc"), NULL);
	i = -1;
	while (env_list->key[++i] != '\0')
		env[i] = env_list->key[i];
	env[i++] = '=';
	j = 0;
	while (env_list->value[j] != '\0')
		env[i++] = env_list->value[j++];
	env[i] = '\0';
	return (env);
}

char	**env_to_environ(t_env *env_list)
{
	char	**environ;
	size_t	i;

	if (!env_list)
		return (NULL);
	environ = (char **)malloc(sizeof(char *) * (ft_env_size(env_list) + 1));
	if (!environ)
		return (perror("malloc"), NULL);
	i = 0;
	while (env_list)
	{
		environ[i] = ft_env_join(env_list);
		if (!environ[i])
			return (free_argv(environ, i), NULL);
		env_list = env_list->next;
		i++;
	}
	environ[i] = NULL;
	return (environ);
}

void read_heredoc(t_fd *heredoc_delimiter, int fd)
{
    char *line;

    while(1)
    {
        line = readline("> ");
        if(!line || ft_strcmp(line, heredoc_delimiter->value) == 0)
            return (free(line));
        else if(fd != -1)
        {
            write(fd, line, ft_strlen(line));
            write(fd, "\n", 1);
        }
        free(line);
    }
}

int parse_heredoc(t_fd *heredoc_delimiter, int fd)
{
    char *line;

    if(!heredoc_delimiter)
        return (0);
    while(heredoc_delimiter->next)
    {
        read_heredoc(heredoc_delimiter, -1);
        heredoc_delimiter = heredoc_delimiter->next;
    }
    read_heredoc(heredoc_delimiter, fd);
    heredoc_delimiter->fd = fd;
}

int	ft_heredoc(t_cmd *cmd)
{
	int		fd[2];
	int		wstatus;
	pid_t	pid;

	if (pipe(fd) == -1)
		return (perror("pipe"), -1);
	pid = fork();
	if (pid < 0)
		return (perror("fork"), -1);
	if (pid == 0)
	{
		close(fd[0]);
		if (parse_heredoc(cmd->heredoc_delimiter, fd[1]) == -1)
			exit(EXIT_FAILURE);
		close(fd[1]);
		exit(EXIT_SUCCESS);
	}
	close(fd[1]);
	waitpid(pid, &wstatus, 0);
	if (WIFSIGNALED(wstatus))
		return (-1);
    return 0;
}

void err_msg(char *value, char *msg)
{
    write(STDERR_FILENO, "minishell: ", 11);
    write(STDERR_FILENO, value, ft_strlen(value));
    write(STDERR_FILENO, msg, ft_strlen(msg));
}

void ft_open_fd_in(t_cmd *cmd, t_fd *current)
{
    if(current->type == TOKEN_REDIR_IN)
    {
        current->fd = open(current->value, O_RDONLY);
        if(errno == ENOENT)
            err_msg(current->value, ": No such file or directory\n");
        else if(errno == EACCES)
            err_msg(current->value, ": Permission denied\n");
        else if(current->fd < 0)
            err_msg(current->value, ": Failed to open file\n");
    }
    cmd->fd_in = current->fd;
}

void ft_open_fd_out(t_cmd *cmd, t_fd *current)
{
    if(current->type == TOKEN_REDIR_OUT)
    {
        current->fd = open(current->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(errno == ENOENT)
            err_msg(current->value, ": No such file or directory\n");
        else if(errno == EACCES)
            err_msg(current->value, ": Permission denied\n");
        else if(current->fd < 0)
            err_msg(current->value, ": Failed to open file\n");
    }
    else if(current->type == TOKEN_APPEND)
    {
        current->fd = open(current->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if(errno == ENOENT)
            err_msg(current->value, ": No such file or directory\n");
        else if(errno == EACCES)
            err_msg(current->value, ": Permission denied\n");
        else if(current->fd < 0)
            err_msg(current->value, ": Failed to open file\n");
    }
    cmd->fd_out = current->fd;
}


int ft_file_redirection(t_cmd *cmd)
{
    t_fd	*current;

    if (!cmd->fds)
        return (0);
    current = cmd->fds;
    while (current)
    {
        if (current->type == TOKEN_REDIR_IN || current->type == TOKEN_HEREDOC)
            ft_open_fd_in(cmd, current);
        else if(current->type == TOKEN_REDIR_OUT || current->type == TOKEN_APPEND)
            ft_open_fd_out(cmd, current);
        if (cmd->fd_in == -1 || cmd->fd_out == -1)
            return (-1);
        current = current->next;
    }
    return (0);
}

void	exec_pipe(t_node *ast, t_env *env_list, int *status)
{
	int		fd[2];
	pid_t	pid;

	if (pipe(fd) == -1)
	{
		*status = -1;
		return (perror("pipe"));
	}
	pid = fork();
	if (pid < 0)
	{
		*status = -1;
		return (perror("fork"));
	}
	if (pid == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		executor(ast->lhs, env_list, status);
		exit(*status);
	}
	else
	{
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		close(fd[1]);
		waitpid(pid, status, 0);
		if (WIFEXITED(*status))
			*status = WEXITSTATUS(*status);
		else if (WIFSIGNALED(*status))
			*status = 128 + WTERMSIG(*status);
        executor(ast->rhs, env_list, status);
        if (WIFEXITED(*status))
            *status = WEXITSTATUS(*status);
        else if (WIFSIGNALED(*status))
            *status = 128 + WTERMSIG(*status);
	}
}

void	ft_execve(t_env *env_list, t_cmd *cmd, int *status)
{
	char	*path;
	char	**environ;
	char	**argv;

    reset_default_signal();
	if (ft_heredoc(cmd) == -1)
		exit(EXIT_FAILURE);
	if (ft_file_redirection(cmd) == -1)
		exit(EXIT_FAILURE);
	if (!cmd->argv || !cmd->argv->value)
		exit(0);
	path = get_path(cmd->argv->value, env_list);
	argv = to_list(cmd->argv);
	environ = env_to_environ(env_list);
    if(g_status != 0)
        return (free(path), free_str_list(argv), free_str_list(environ), exit(128 + g_status));
	if (!path || !argv || !environ)
		execve(path, argv, environ);
	if (errno == ENOENT)
	{
		*status = 127;
		return err_msg(cmd->argv->value, ": command not found\n");
	}
	else if (errno == EACCES)
	{
		*status = 126;
		return err_msg(cmd->argv->value, ": permission denied\n");
	}
	return err_msg(cmd->argv->value, ": execve failed\n");
	free(path);
	free_str_list(argv);
	free_str_list(environ);
	exit(EXIT_FAILURE);
}

void	exec_cmd(t_env *env_list, t_cmd *cmd, int *status)
{
	pid_t	pid;
	int		wstatus;

	if (is_builtin(cmd->argv->value))
		return (exec_builtin(env_list, cmd, status));
	pid = fork();
	if (pid < 0)
	{
		*status = -1;
		return (perror("fork"));
	}
	if (pid == 0)
		ft_execve(env_list, cmd, status);
	else
	{
		free_cmd(cmd);
		if (waitpid(pid, &wstatus, 0) == -1)
		{
			*status = 1;
			return (perror("waitpid"));
		}
		if (WIFEXITED(wstatus))
			*status = WEXITSTATUS(wstatus);
		else if (WIFSIGNALED(wstatus))
			*status = 128 + WTERMSIG(wstatus);
	}
}

void	executor(t_node *ast, t_env *env_list, int *status)
{
	if (ast->type == NODE_PIPE && g_status == 0)
		exec_pipe(ast, env_list, status);
	else if (ast->type == NODE_AND_IF && g_status == 0)
	{
		executor(ast->lhs, env_list, status);
		if (*status == 0 && g_status == 0)
			executor(ast->rhs, env_list, status);
	}
	else if (ast->type == NODE_OR_IF && g_status == 0)
	{
		executor(ast->lhs, env_list, status);
		if (*status != 0 && g_status == 0)
			executor(ast->rhs, env_list, status);
	}
	else if (ast->type == NODE_CMD && g_status == 0)
		exec_cmd(env_list, ast->cmd, status);
}
