#include "minishell.h"
#include <readline/readline.h>
#include <readline/history.h>

size_t ft_strlen(const char *s)
{
    size_t len = 0;
    while (s[len])
        len++;
    return len;
}

char *ft_strdup(const char *s)
{
    size_t len = ft_strlen(s);
    char *copy = (char *)malloc(len + 1);
    if (!copy)
        return NULL;
    for (size_t i = 0; i < len; i++)
        copy[i] = s[i];
    copy[len] = '\0';
    return copy;
}

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
    while (n-- && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return (n == (size_t)-1) ? 0 : *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *ft_strchr(const char *s, int c)
{
    while (*s)
    {
        if (*s == (char)c)
            return (char *)s;
        s++;
    }
    return (c == '\0') ? (char *)s : NULL;
}

char *ft_strjoin(const char *s1, const char *s2)
{
    size_t len1 = ft_strlen(s1);
    size_t len2 = ft_strlen(s2);
    char *res = malloc(len1 + len2 + 1);
    if (!res)
        return NULL;
    for (size_t i = 0; i < len1; i++)
        res[i] = s1[i];
    for (size_t j = 0; j < len2; j++)
        res[len1 + j] = s2[j];
    res[len1 + len2] = '\0';
    return res;
}

void handle_sigint(int sig)
{
    (void)sig;
    g_signal = 1;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void init_signals(void)
{
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}

int main(void)
{
    char *line;
    t_token *tokens;
    t_node *ast;

    init_signals();
    while (1)
    {
        line = readline("minishell> ");
        if (!line)
        {
            write(1, "exit\n", 5);
            break;
        }
        if (*line)
        {
            add_history(line);
            tokens = tokenize(line);
            if (tokens)
            {
                ast = parse(tokens);
                expand_ast(ast);
                execute_ast(ast);
                free_ast(ast);
            }
        }
        free(line);
    }
    return 0;
}
