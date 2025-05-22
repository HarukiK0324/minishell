#include "minishell.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

unsigned int	count_words(const char *s, char c)
{
	unsigned int	count;
	unsigned int	in_word;

	count = 0;
	in_word = 0;
	while (*s != '\0')
	{
		if (*s == c)
			in_word = 0;
		else if (!in_word)
		{
			in_word = 1;
			count++;
		}
		s++;
	}
	return (count);
}

char	*substring(char const *s, unsigned int index, char c)
{
	unsigned int	len;
	unsigned int	i;
	char			*str;

	len = 0;
	i = 0;
	while (s[index + len] != '\0' && s[index + len] != c)
		len++;
	str = malloc(len + 1);
	if (str == NULL)
		return (NULL);
	while (i < len)
	{
		str[i] = s[index + i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

void	free_all(char **arr, unsigned int i)
{
	unsigned int	index;

	index = 0;
	while (index < i)
	{
		free(arr[index]);
		index++;
	}
	free(arr);
}

char	**ft_split(char const *s, char c)
{
	char			**arr;
	unsigned int	i;
	unsigned int	index;

	i = 0;
	index = 0;
	arr = malloc((count_words(s, c) + 1) * sizeof(char *));
	while (i < count_words(s, c))
	{
		while (s[index] == c)
			index++;
		arr[i] = substring(s, index, c);
		if (arr[i] == NULL)
		{
			free_all(arr, i);
			return (NULL);
		}
		index += ft_strlen(arr[i]);
		i++;
	}
	arr[i] = NULL;
	return (arr);
}

char	*ft_access(char *path, char *cmd)
{
	char	*tmp;

	tmp = ft_strjoin(path, "/");
	path = ft_strjoin(tmp, cmd);
	if (tmp)
		free(tmp);
	if (access(path, F_OK) == 0)
		return (path);
	if (path)
		free(path);
	return (NULL);
}

int is_builtin(t_token *t)
{
    if (!t || t->type != TOKEN_WORD)
        return 0;
    if (ft_strcmp(t->value, "echo") == 0)
        return 1;
    if (ft_strcmp(t->value, "cd") == 0)
        return 1;
    if (ft_strcmp(t->value, "exit") == 0)
        return 1;
    return 0;
}

int builtin_cd(t_token *t)
{
    const char *target = NULL;
    if (!t->next || !t->next->value) {
        target = getenv("HOME");
        if (!target) {
            write(2, "minishell: cd: HOME not set\n", 28);
            return 1;
        }
    } else {
        target = t->next->value;
    }
    if (chdir(target) != 0) {
        perror("minishell: cd");
        return 1;
    }
    return 0;
}

void builtin_echo(t_token *t)
{
    t = t->next;
    while (t && t->type == TOKEN_WORD)
    {
        write(STDOUT_FILENO, t->value, ft_strlen(t->value));
        if (t->next && t->next->type == TOKEN_WORD)
            write(STDOUT_FILENO, " ", 1);
        t = t->next;
    }
    write(STDOUT_FILENO, "\n", 1);
}

int handle_builtin(t_token *t)
{
    if (ft_strcmp(t->value, "exit") == 0)
        exit(0);
    else if (ft_strcmp(t->value, "cd") == 0)
        return builtin_cd(t);
    else if (ft_strcmp(t->value, "echo") == 0)
    {
        builtin_echo(t);
        return 0;
    }
    return 1;
}

void setup_redirects(t_token *t)
{
    while (t)
    {
        int fd;
        if (t->type == TOKEN_REDIR_IN && t->next)
        {
            fd = open(t->next->value, O_RDONLY);
            if (fd < 0) {
                perror(t->next->value);
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        else if (t->type == TOKEN_REDIR_OUT && t->next)
        {
            fd = open(t->next->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror(t->next->value);
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (t->type == TOKEN_APPEND && t->next)
        {
            fd = open(t->next->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror(t->next->value);
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        // << (heredoc) not implemented yet
        t = t->next;
    }
}

char **token_to_argv(t_token *t)
{
    int count = 0;
    t_token *tmp = t;
    while (tmp)
    {
        if (tmp->type == TOKEN_WORD)
            count++;
        tmp = tmp->next;
    }
    char **argv = malloc(sizeof(char *) * (count + 1));
    if (!argv)
        return NULL;
    count = 0;
    while (t)
    {
        if (t->type == TOKEN_WORD)
            argv[count++] = t->value;
        t = t->next;
    }
    argv[count] = NULL;
    return argv;
}

char	*get_path(char *argv, char *environ[])
{
	char	**paths;
	int		i;
	char	*path;

	i = 0;
	while (*(++environ) != NULL)
	{
		if (ft_strncmp(*environ, "PATH=", 5) == 0)
			break ;
	}
	if (*environ == NULL)
        perror(ft_strjoin("minishell: ",argv));
	paths = ft_split(*environ + 5, ':');
	path = NULL;
	while (paths && paths[i] != NULL && path == NULL)
		path = ft_access(paths[i++], argv);
	i = 0;
	while (paths[i])
		free(paths[i++]);
	free(paths);
	if (path == NULL)
        perror(ft_strjoin("minishell: ",argv));
	return (path);
}

int exec_cmd(t_token *t)
{
    if (is_builtin(t))
        return handle_builtin(t);

    pid_t pid = fork();
    if (pid == 0)
    {
        setup_redirects(t);
        char **argv = token_to_argv(t);
        if (argv && argv[0]) {
            extern char **environ;
            char *path;
            if (ft_strchr(argv[0], '/'))
                path = argv[0];
            else
                path = get_path(argv[0], environ);
            execve(path, argv, environ);
        }
        perror(ft_strjoin("minishell: ",argv[0]));
        free(argv);
        exit(1);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int exec_pipe(t_node *node)
{
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }
    pid_t pid = fork();

    if (pid == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        exit(execute_ast(node->lhs));
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        exit(execute_ast(node->rhs));
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid, NULL, 0);
    int status = 0;
    waitpid(pid2, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int execute_ast(t_node *node)
{
    if (!node)
        return 0;

    if (node->type == NODE_CMD)
        return exec_cmd(node->tokens);

    if (node->type == NODE_PIPE)
        return exec_pipe(node);

    if (node->type == NODE_AND_IF)
    {
        int status = execute_ast(node->lhs);
        if (status == 0)
            return execute_ast(node->rhs);
        return status;
    }

    if (node->type == NODE_OR_IF)
    {
        int status = execute_ast(node->lhs);
        if (status != 0)
            return execute_ast(node->rhs);
        return status;
    }

    if (node->type == NODE_SEMICOLON)
    {
        execute_ast(node->lhs);
        return execute_ast(node->rhs);
    }

    return 1;
}
