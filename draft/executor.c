#include "minishell.h"
#include <fcntl.h>
#include <sys/wait.h>

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
    if (!t->next || !t->next->value)
        return chdir(getenv("HOME"));
    return chdir(t->next->value);
}

void builtin_echo(t_token *t)
{
    t = t->next;
    while (t && t->type == TOKEN_WORD)
    {
        write(1, t->value, ft_strlen(t->value));
        if (t->next && t->next->type == TOKEN_WORD)
            write(1, " ", 1);
        t = t->next;
    }
    write(1, "\n", 1);
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
        if (t->type == TOKEN_REDIR_IN && t->next)
        {
            int fd = open(t->next->value, O_RDONLY);
            if (fd >= 0)
            {
                dup2(fd, 0);
                close(fd);
            }
        }
        else if (t->type == TOKEN_REDIR_OUT && t->next)
        {
            int fd = open(t->next->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd >= 0)
            {
                dup2(fd, 1);
                close(fd);
            }
        }
        else if (t->type == TOKEN_APPEND && t->next)
        {
            int fd = open(t->next->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd >= 0)
            {
                dup2(fd, 1);
                close(fd);
            }
        }
        // NOTE: For simplicity, << (heredoc) not implemented here
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

int exec_cmd(t_token *t)
{
    if (is_builtin(t))
        return handle_builtin(t);

    pid_t pid = fork();
    if (pid == 0)
    {
        setup_redirects(t);
        char **argv = token_to_argv(t);
        if (argv && argv[0])
            execve(argv[0], argv, NULL);
        perror("execve");
        exit(1);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int exec_pipe(t_node *node)
{
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();

    if (pid == 0)
    {
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        close(pipefd[1]);
        exit(execute_ast(node->lhs));
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        dup2(pipefd[0], 0);
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
