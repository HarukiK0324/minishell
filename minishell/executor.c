#include "minishell.h"

void executor(t_node *ast, t_env *env_list, int *status)
{
    if(ast->type == NODE_PIPE)
        pipe();
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
        exec_cmd();
}