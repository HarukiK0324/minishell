#include "minishell.h"

int iscommand(TokenType type)
{
    return (type == TOKEN_WORD || type == TOKEN_DOLLAR ||  type == TOKEN_SINGLE_QUOTE || type == TOKEN_DOUBLE_QUOTE || type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT ||
            type == TOKEN_HEREDOC || type == TOKEN_APPEND);
}

t_node *parser(t_token *token)
{
    t_node *node;
    t_token *current;

    node = NULL;
    while(current)
    {
        if(iscommand(current->type))
        {
            t_node *cmd_node;
            cmd_node = (t_node *)malloc(sizeof(t_node));
            if (!cmd_node)
                return (NULL);
            cmd_node->type = NODE_CMD;
            cmd_node->tokens = NULL;
            cmd_node->lhs = NULL;
            cmd_node->rhs = NULL;
            while(current && iscommand(current->type))
            {
                append_token(&cmd_node->tokens, current);
                current = current->next;
            }
        }
        else if(current->type == TOKEN_PIPE)
            ;
        else
            ;
    }
    return (node);
}
