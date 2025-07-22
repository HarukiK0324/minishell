#include "minishell.h"

int is_builtin(char *cmd)
{
    if(ft_strcmp(cmd, "cd") == 0 ||
       ft_strcmp(cmd, "echo") == 0 ||
       ft_strcmp(cmd, "exit") == 0 ||
       ft_strcmp(cmd, "export") == 0 ||
       ft_strcmp(cmd, "pwd") == 0 ||
       ft_strcmp(cmd, "unset") == 0 ||
       ft_strcmp(cmd, "env") == 0)
        return 1;
}

int exec_builtin(t_env *env_list, t_cmd *cmd)
{
    if(ft_strcmp(cmd->argv->value, "cd") == 0)
        return exec_cd(cmd->argv, env_list);
    else if(ft_strcmp(cmd->argv->value, "echo") == 0)
        return exec_echo(cmd->argv);
    else if(ft_strcmp(cmd->argv->value, "exit") == 0)
        return exec_exit(cmd->argv);
    else if(ft_strcmp(cmd->argv->value, "export") == 0)
        return exec_export(cmd->argv, env_list);
    else if(ft_strcmp(cmd->argv->value, "pwd") == 0)
        return exec_pwd();
    else if(ft_strcmp(cmd->argv->value, "unset") == 0)
        return exec_unset(cmd->argv, env_list);
    else if(ft_strcmp(cmd->argv->value, "env") == 0)
        return exec_env(env_list);
}

void exec_pipe(t_node *ast, t_env *env_list, int *status)
{
    int fd[2];
    pid_t pid;

    if(pipe(fd) == -1)
    {
        *status = -1;
        return perror("pipe");
    }
    pid = fork();
    if(pid < 0)
    {
        *status = -1;
        return perror("fork");
    }
    if(pid == 0)
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
        if(WIFEXITED(*status))
            *status = WEXITSTATUS(*status);
        else if(WIFSIGNALED(*status))
            *status = 128 + WTERMSIG(*status);
    }
}

void ft_execve(t_env *env_list, t_cmd *cmd, int *status)
{
    if(ft_heredoc(cmd->heredoc_delimiter) == -1)
        exit(EXIT_FAILURE);
    if(ft_file_redirection(cmd->fds) == -1)
        exit(EXIT_FAILURE);
    if(!cmd->argv || !cmd->argv->value)
        exit(0);
    execve(get_path(cmd->argv[0], env_list), cmd->argv, env_to_environ(env_list));
    if (errno == ENOENT)
    {
        write(STDERR_FILENO, "minishell: ", 11);
        write(STDERR_FILENO, cmd->argv->value, ft_strlen(cmd->argv->value));
        write(STDERR_FILENO, ": command not found\n", 20);
        *status = 127;
        return;
    }
    else if (errno == EACCES)
    {
        write(STDERR_FILENO, "minishell: ", 11);
        write(STDERR_FILENO, cmd->argv->value, ft_strlen(cmd->argv->value));
        write(STDERR_FILENO, ": permission denied\n", 20);
        *status = 126;
        return;
    }
}

void exec_cmd(t_env *env_list, t_cmd *cmd, int *status)
{
    pid_t pid;
    int wstatus;

    if(is_builtin(cmd->argv->value))
    {
        *status = exec_builtin(env_list, cmd);
        return;
    }
    pid = fork();
    if(pid < 0)
    {
        *status = -1;
        return perror("fork");
    }
    if(pid == 0)
        ft_execve(env_list, cmd, status);
    else
    {
        if(waitpid(pid, &wstatus, 0) == -1)
        {
            *status = -1;
            return perror("waitpid");
        }
        if(WIFEXITED(wstatus))
            *status = WEXITSTATUS(wstatus);
        else if(WIFSIGNALED(wstatus))
            *status = 128 + WTERMSIG(wstatus);
        else
            *status = 0;
    }
}

void executor(t_node *ast, t_env *env_list, int *status)
{
    if(ast->type == NODE_PIPE)
        exec_pipe(ast, env_list, status);
    else if(ast->type == NODE_AND_IF)
    {
        executor(ast->lhs, env_list, status);
        if(*status == 0)
            executor(ast->rhs, env_list, status);
    }
    else if(ast->type == NODE_OR_IF)
    {
        executor(ast->lhs, env_list, status);
        if(*status != 0)
            executor(ast->rhs, env_list, status);
    }else if(ast->type == NODE_CMD)
        exec_cmd(env_list, ast->cmd, status);
}
