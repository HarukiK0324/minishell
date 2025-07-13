#include "minishell.h"

int ischar(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
}

char *replace_env_var(char *str,int *j)
{
    int i;
    int k;
    char *env_var;

    if(str[*j + 1] == '?')
        return replace_status(g_signal);
    i = 0;
    while(str[*j + 1 + i] != '\0' && ischar(str[*j + 1 + i]))
        i++;
    if(i > 0)
    {
        env_var = (char *)malloc(i + 1);
        if (!env_var)
            return (perror("malloc"), NULL);
        k = 0;
        while(k < i)
        {
            env_var[k] = str[*j + 1 + k];
            k++;
        }
        env_var[k] = '\0';
        
    }
    
}

char *trim_quote(char *str,int *j,char c)
{
    int i;
    char *new_str;

    i = 0;
    new_str = (char *)malloc(ft_strlen(str) - 1);
    while(i < *j)
    {
        new_str[i] = str[i];
        i++;
    }
    while(str[i + 1] != c)
    {
        new_str[i] = str[i + 1];
        (*j)++;
        i++;
    }
    while(str[i + 2] != '\0')
    {
        new_str[i] = str[i + 2];
        i++;
    }
    new_str[i] = '\0';
    free(str);
    return new_str;
}

char *trim_double_quote(char *str,int *j)
{
    int i;
    char *new_str;

    i = *j + 1;
    while(str[i] != '"')
    {
        if(str[i] == '$')
            str = replace_env_var(str,&i);
        else
            i++;
    }
    return trim_quote(str,j,'"');
}

void expand_cmd(t_cmd *cmd)
{
    int i;
    int j;

    i = 0;
    while(cmd->argv && cmd->argv[i])
    {
        j = 0;
        while(cmd->argv[i][j] != '\0')
        {
            if(cmd->argv[i][j] == '\'')
                cmd->argv[i] = trim_quote(cmd->argv[i],&j,'\'');
            else if(cmd->argv[i][j] == '"')
                cmd->argv[i] = trim_double_quote(cmd->argv[i],&j);
            else if(cmd->argv[i][j] == '$')
                cmd->argv[i] = replace_env_var(cmd->argv[i],&j);
            else
                j++;
        }
        i++;
    }
}

int expander(t_node *node)
{
    if(!node)
        return NULL;
    if(node->type == NODE_CMD)
        expand_cmd(node->cmd);
    else
    {
        if(node->lhs)
            expander(node->lhs);
        if(node->rhs)
            expander(node->rhs);
    }
}
