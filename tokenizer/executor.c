#include "minishell.h"

void executor(t_node *node)
{
    if(node->type == NODE_CMD)
        ft_exec(node->tokens);
    else if(node->type == NODE_PIPE)
    {
        if(node->lhs)
        executor(node->lhs);
        if(node->rhs)
            executor(node->rhs);
    }
    else if(node->type == NODE_SEMICOLON)
    {
        if(node->lhs)
            executor(node->lhs);
        if(node->rhs)
            executor(node->rhs);
    }
    else if(node->type == NODE_AND_IF)
    {
        if(node->lhs)
            executor(node->lhs);
        if(node->rhs)
            executor(node->rhs);
    }
    else if(node->type == NODE_OR_IF)
    {
        if(node->lhs)
            executor(node->lhs);
        if(node->rhs)
            executor(node->rhs);
    }
}