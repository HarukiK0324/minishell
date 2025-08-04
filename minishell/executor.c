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

void exec_builtin(t_env *env_list, t_cmd *cmd, int *status)
{
	int original_stdin;
	int original_stdout;
	
	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
    if (original_stdin == -1 || original_stdout == -1)
    {
        perror("dup");
        *status = 1;
        return;
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
        return;
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
	while (env_list && ft_strcmp(env_list->key, "PATH") != 0)
		env_list = env_list->next;
	if (!env_list)
		return (NULL);
	paths = ft_split(env_list->value, ':');
	path = NULL;
	while (paths && paths[i])
	{
		path = ft_access(paths[i++], argv);
		if(path)
			break;
	}
	free_str_list(paths);
	if (path)
		return (path);
	return (NULL);
}

char	*get_path(char *cmd, t_env *env_list)
{
	if (ft_strchar(cmd, '/') != ft_strlen(cmd))
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

void	read_heredoc(t_fd *heredoc_delimiter, int fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if(!line)
			return ;
		if (ft_strcmp(line, heredoc_delimiter->value) == 0)
			return (free(line));
		else
		{
			write(fd, line, ft_strlen(line));
			write(fd, "\n", 1);
		}
		free(line);
	}
}

void	parse_heredoc(t_fd *heredoc_delimiter, int fd_in, int fd_out)
{
	if (!heredoc_delimiter)
		return ;
	while (heredoc_delimiter->next)
	{
		read_heredoc(heredoc_delimiter, -1);
		heredoc_delimiter = heredoc_delimiter->next;
		heredoc_delimiter->fd = 0;
	}
	read_heredoc(heredoc_delimiter, fd_in);
	heredoc_delimiter->fd = fd_out;
}

int	ft_heredoc(t_cmd *cmd)
{
	int		fd[2];
	int		wstatus;
	pid_t	pid;

	if (!cmd->heredoc_delimiter)
		return (0);
	if (pipe(fd) == -1)
		return (perror("pipe"), -1);
	pid = fork();
	if (pid < 0)
		return (perror("fork"), -1);
	if (pid == 0)
	{
		close(fd[0]);
		parse_heredoc(cmd->heredoc_delimiter, fd[1],fd[0]);
		close(fd[1]);
		exit(EXIT_SUCCESS);
	}
	close(fd[1]);
	waitpid(pid, &wstatus, 0);
	if (WIFSIGNALED(wstatus))
		return (-1);
	return (0);
}

void	err_msg(char *value, char *msg)
{
	write(STDERR_FILENO, "minishell: ", 11);
	write(STDERR_FILENO, value, ft_strlen(value));
	write(STDERR_FILENO, msg, ft_strlen(msg));
}

int	ft_open_fd_in(int *fd, t_fd *current)
{
	if (current->type == TOKEN_REDIR_IN)
	{
		*fd = open(current->value, O_RDONLY);
		if (errno == ENOENT)
			return(err_msg(current->value, ": No such file or directory\n"),-1);
		else if (errno == EACCES)
		return(err_msg(current->value, ": Permission denied\n"),-1);
		else if (*fd < 0)
			return(err_msg(current->value, ": Failed to open file\n"),-1);
	}
	return (0);
}

int	ft_open_fd_out(int *fd, t_fd *current)
{
	if (current->type == TOKEN_REDIR_OUT)
		*fd = open(current->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (current->type == TOKEN_APPEND)
		*fd = open(current->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (errno == ENOENT)
		return(err_msg(current->value, ": No such file or directory\n"),-1);
	else if (errno == EACCES)
		return(err_msg(current->value, ": Permission denied\n"),-1);
	else if (*fd < 0)
		return(err_msg(current->value, ": Failed to open file\n"),-1);
	return (0);
}

int process_redirections(t_cmd *cmd)
{
    // Step 1: Count how many heredocs we have
    int heredoc_count = 0;
    t_fd *curr_heredoc = cmd->heredoc_delimiter;
    while (curr_heredoc)
    {
        heredoc_count++;
        curr_heredoc = curr_heredoc->next;
    }
    if (heredoc_count > 0)
    {
        // Step 2: Create an array to store prepared heredoc fds
        int *heredoc_fds = malloc(sizeof(int) * heredoc_count);
        if (!heredoc_fds)
            return (perror("malloc"), -1);
            
        // Step 3: Prepare all heredocs and store their fds in order
        curr_heredoc = cmd->heredoc_delimiter;
        for (int i = 0; i < heredoc_count; i++)
        {
            int pipe_fds[2];
            if (pipe(pipe_fds) == -1)
            {
                // Clean up already created pipes
                for (int j = 0; j < i; j++)
                    close(heredoc_fds[j]);
                free(heredoc_fds);
                return (perror("pipe"), -1);
            }
            
            pid_t pid = fork();
            if (pid < 0)
            {
                // Clean up all pipes
                close(pipe_fds[0]);
                close(pipe_fds[1]);
                for (int j = 0; j < i; j++)
                    close(heredoc_fds[j]);
                free(heredoc_fds);
                return (perror("fork"), -1);
            }
            
            if (pid == 0)
            {
                // Child process
                reset_default_signal();
                close(pipe_fds[0]);
                read_heredoc(curr_heredoc, pipe_fds[1]);
                close(pipe_fds[1]);
                exit(EXIT_SUCCESS);
            }
            close(pipe_fds[1]);
            int wstatus;
            waitpid(pid, &wstatus, 0);
            if (WIFSIGNALED(wstatus))
            {
                close(pipe_fds[0]);
                for (int j = 0; j < i; j++)
                    close(heredoc_fds[j]);
                free(heredoc_fds);
                return -1;
            }
            heredoc_fds[i] = pipe_fds[0];
            curr_heredoc = curr_heredoc->next;
        }
        
        // Step 4: Process all redirections in original order
        int heredoc_index = 0;
        t_fd *current = cmd->fds;
        while (current)
        {
            if (current->type == TOKEN_REDIR_IN)
            {
                int new_fd;
                if (ft_open_fd_in(&new_fd, current) < 0)
                {
                    for (int j = 0; j < heredoc_count; j++)
                        close(heredoc_fds[j]);
                    free(heredoc_fds);
                    return -1;
                }
                if (cmd->fd_in != 0)
                    close(cmd->fd_in);
                cmd->fd_in = new_fd;
            }
            else if (current->type == TOKEN_HEREDOC)
            {
				if (cmd->fd_in != 0)
					close(cmd->fd_in);
				cmd->fd_in = heredoc_fds[heredoc_index++];
            }
            else if (current->type == TOKEN_REDIR_OUT || current->type == TOKEN_APPEND)
            {
                // Output redirection handling...
                int new_fd;
                if (ft_open_fd_out(&new_fd, current) < 0)
                {
                    for (int j = 0; j < heredoc_count; j++)
                        close(heredoc_fds[j]);
                    free(heredoc_fds);
                    return -1;
                }
                if (cmd->fd_out != 1)
                    close(cmd->fd_out);
                cmd->fd_out = new_fd;
            }
            current = current->next;
        }
        
        // Close any unused heredoc fds
        for (int i = heredoc_index; i < heredoc_count; i++)
            close(heredoc_fds[i]);
        free(heredoc_fds);
    }
    else
    {
        // No heredocs, just process regular redirections
        t_fd *current = cmd->fds;
        while (current)
        {
            if (current->type == TOKEN_REDIR_IN)
            {
                int new_fd;
                if (ft_open_fd_in(&new_fd, current) < 0)
                    return -1;
                if (cmd->fd_in != 0)
                    close(cmd->fd_in);
                cmd->fd_in = new_fd;
            }
            else if (current->type == TOKEN_REDIR_OUT || current->type == TOKEN_APPEND)
            {
                // Output redirection handling...
                int new_fd;
                if (ft_open_fd_out(&new_fd, current) < 0)
                    return -1;
                if (cmd->fd_out != 1)
                    close(cmd->fd_out);
                cmd->fd_out = new_fd;
            }
            current = current->next;
        }
    }
    return 0;
}

// void exec_pipe(t_node *ast, t_env *env_list, int *status)
// {
//     int fd[2];
//     pid_t pid1;
// 	pid_t pid2;
//     int status1;
// 	int status2;

//     if (pipe(fd) == -1)
//     {
//         *status = -1;
//         return (perror("pipe"));
//     }
//     pid1 = fork();
//     if (pid1 < 0)
//     {
//         *status = -1;
//         return (perror("fork"));
//     }
//     if (pid1 == 0)
//     {
//         dup2(fd[1], STDOUT_FILENO);
//         close(fd[0]);
//         close(fd[1]);
//         executor(ast->lhs, env_list, status);
//         exit(*status);
//     }
//     pid2 = fork();
//     if (pid2 < 0)
//     {
//         *status = -1;
//         return (perror("fork"));
//     }
//     if (pid2 == 0)
//     {
//         dup2(fd[0], STDIN_FILENO);
//         close(fd[0]);
//         close(fd[1]);
//         executor(ast->rhs, env_list, status);
//         exit(*status);
//     }
//     close(fd[0]);
//     close(fd[1]);
//     waitpid(pid1, &status1, 0);
//     waitpid(pid2, &status2, 0);
//     if (WIFEXITED(status2))
//         *status = WEXITSTATUS(status2);
//     else if (WIFSIGNALED(status2))
//         *status = 128 + WTERMSIG(status2);
// }

void exec_pipe(t_node *ast, t_env *env_list, int *status)
{
    int fd[2];
    pid_t pid1, pid2;
    int status1, status2;

    if (pipe(fd) == -1)
    {
        *status = -1;
        perror("pipe");
        return;
    }

    // Left side
    pid1 = fork();
    if (pid1 < 0)
    {
        *status = -1;
        perror("fork");
        return;
    }
    if (pid1 == 0)
    {
        // Handle heredocs and file redirections for the left command
        if (ast->lhs->type == NODE_CMD)
        {
            t_cmd *cmd = ast->lhs->cmd;
            if (process_redirections(cmd) == -1)
                exit(1);
            if (cmd->fd_in != 0)
            {
                dup2(cmd->fd_in, STDIN_FILENO);
                close(cmd->fd_in);
            }
            if (cmd->fd_out != 1)
            {
                dup2(cmd->fd_out, STDOUT_FILENO);
                close(cmd->fd_out);
            }
        }
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        executor(ast->lhs, env_list, status);
        exit(*status);
    }

    // Right side
    pid2 = fork();
    if (pid2 < 0)
    {
        *status = -1;
        perror("fork");
        return;
    }
    if (pid2 == 0)
    {
        // Handle heredocs and file redirections for the right command
        if (ast->rhs->type == NODE_CMD)
        {
            t_cmd *cmd = ast->rhs->cmd;
            if (process_redirections(cmd) == -1)
                exit(1);
            if (cmd->fd_in != 0)
            {
                dup2(cmd->fd_in, STDIN_FILENO);
                close(cmd->fd_in);
            }
            if (cmd->fd_out != 1)
            {
                dup2(cmd->fd_out, STDOUT_FILENO);
                close(cmd->fd_out);
            }
        }
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        executor(ast->rhs, env_list, status);
        exit(*status);
    }

    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    if (WIFEXITED(status2))
        *status = WEXITSTATUS(status2);
    else if (WIFSIGNALED(status2))
        *status = 128 + WTERMSIG(status2);
}

void	ft_execve(t_env *env_list, t_cmd *cmd, int *status)
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
		{
			perror("dup2 input redirection failed");
			exit(EXIT_FAILURE);
		}
		close(cmd->fd_in);
	}
	if (cmd->fd_out != 1)
	{
		if (dup2(cmd->fd_out, STDOUT_FILENO) == -1)
		{
			perror("dup2 output redirection failed");
			exit(EXIT_FAILURE);
		}
		close(cmd->fd_out);
	}
	if (!cmd->argv || !cmd->argv->value)
		exit(0);
	path = get_path(cmd->argv->value, env_list);
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
	{
		*status = 127;
		err_msg(cmd->argv->value, ": command not found\n");
		exit(*status);
	}
	else if (errno == EACCES)
	{
		*status = 126;
		err_msg(cmd->argv->value, ": permission denied\n");
		exit(*status);
	}
	err_msg(cmd->argv->value, ": command not founds\n");
	exit(EXIT_FAILURE);
}

void	exec_cmd(t_env *env_list, t_cmd *cmd, int *status)
{
	pid_t	pid;
	int		wstatus;

	if (cmd->argv && is_builtin(cmd->argv->value))
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
		if (waitpid(pid, &wstatus, 0) == -1 && g_status == 0)
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
    if(!ast)
		return ;
	if (ast->type == NODE_PIPE && g_status == 0)
		exec_pipe(ast, env_list, status);
	else if (ast->type == NODE_AND_IF && g_status == 0)
	{
		executor(ast->lhs, env_list, status);
		if (*status == 0 && g_status == 0)
		{
			expander(ast->rhs, env_list, status);
			executor(ast->rhs, env_list, status);
		}
	}
	else if (ast->type == NODE_OR_IF && g_status == 0)
	{
		executor(ast->lhs, env_list, status);
		if (*status != 0 && g_status == 0)
		{
			expander(ast->rhs, env_list, status);
			executor(ast->rhs, env_list, status);
		}
	}
	else if (ast->type == NODE_CMD && g_status == 0)
		exec_cmd(env_list, ast->cmd, status);
}
