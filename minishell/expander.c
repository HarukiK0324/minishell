#include "minishell.h"

char *replace_env_var(char *str)
{
    int i;
    int k;

    i = 0;
    while(str[i] && str[i] != '$')
        i++;
    if(str[i] == '$')
    {
        k = 1;
        if(str[i+k] == '?')

    }
}

char *trim_string(char *str)
{
    int i;
    int j;
    char *trimmed;

    i = 0;
    while(str[i] && str[i] != '\'')
        i++;
    if(str[i] == '\'')
        i++;
    j = i;
    while(str[j] && str[j] != '\'')
        j++;
    if(str[j] == '\'')
        j++;
    trimmed = (char *)malloc(j - i + 1);
    if (!trimmed)
        return NULL;
    for(int k = 0; k < j - i; k++)
        trimmed[k] = str[i + k];
    trimmed[j - i] = '\0';
    return trimmed;
}

void expand_cmd(t_cmd *cmd)
{
    int i;
    int j;

    i = 0;
    while(cmd->argv && cmd->argv[i])
    {
        j = 0;
        while(cmd->argv[i][j])
        {
            if(cmd->argv[i][j] == '\'')
                cmd->argv[i] = trim_single_quote(cmd->argv[i],&j);
            else if(cmd->argv[i][j] == '"')
                cmd->argv[i] = trim_double_quote(cmd->argv[i],&j);
            else if(cmd->argv[i][j] == '$')
                cmd->argv[i] = replace_env_var(cmd->argv[i],&j);
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
