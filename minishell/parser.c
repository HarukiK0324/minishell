#include "minishell.h"

void print_synerr(TokenType expected)
{
    char *token;

    if(expected == TOKEN_END)
        printf("minishell: syntax error: unexpected end of file\n");
    else
    {
        if(expected == TOKEN_CLOSE_PAREN)
        token = ")";
        else if(expected == TOKEN_PIPE)
            token = "|";
        else if(expected == TOKEN_AND_IF)
            token = "&&";
        else if(expected == TOKEN_OR_IF)
            token = "||";
        else if(expected == TOKEN_SEMICOLON)
            token = ";";
        printf("minishell: syntax error near unexpected token '%s'\n",token);
    }
}

t_node *init_node(void)
{
    t_node *node;
    
    node = (t_node *)malloc(sizeof(t_node));
    if (!node)
        return NULL;
    node->type = NULL;
    node->lhs = NULL;
    node->rhs = NULL;
    node->cmd = NULL;
    return node;
}

t_node *parse_semicolon(t_token *tokens)
{
    t_node *node;

    node = init_node();
    if (!node)
        return (perror("malloc"), NULL);
    while(tokens && tokens-> type != TOKEN_SEMICOLON)
    {
        if(tokens->type == TOKEN_AND_IF || tokens->type == TOKEN_OR_IF)
            node = add_condition(&tokens, node);
        else if(tokens->type == TOKEN_OPEN_PAREN)
            node = add_paren(&tokens, node);
        else if(tokens->type == TOKEN_PIPE)
            node = add_pipe(&tokens,node);
        else
            node = add_cmd(&tokens,node);
    }
    return node;
}

t_node *parse_condition(t_token *tokens)
{
    t_node *node;

    node = init_node();
    if (!node)
        return (perror("malloc"), NULL);
    while(tokens && tokens-> type != TOKEN_SEMICOLON && tokens->type != TOKEN_AND_IF && tokens->type != TOKEN_OR_IF)
    {
        if(tokens->type == TOKEN_OPEN_PAREN)
            node = add_paren(&tokens, node);
        else if(tokens->type == TOKEN_PIPE)
            node = add_pipe(&tokens,node);
        else
            node = add_cmd(&tokens,node);
    }
    return node;
}

t_node *parse_pipe(t_token *tokens)
{
    t_node *node;

    node = init_node();
    if (!node)
        return (perror("malloc"), NULL);
    while(tokens && tokens-> type == TOKEN_PIPE)
        node = add_pipe(&tokens,node);
    return node;
}



t_node *add_semicolon(t_token **tokens, t_node *node)
{
    t_node *new_root;

    if(!node->type)
        return(print_synerr(TOKEN_SEMICOLON), NULL);
    new_root = init_node();
    if (!new_root)
        return (perror("malloc"), NULL);
    new_root->lhs = node;
    new_root->type = NODE_SEMICOLON;
    *tokens = &((*tokens)->next);
    new_root->rhs = parse_semicolon(*tokens);
    return new_root;
}

t_node *add_condition(t_token **tokens, t_node *node)
{
    t_node *new_root;

    if(node->type != NODE_CMD || node->type != NODE_PIPE)
        return(print_synerr((*tokens)->type), NULL);
    new_root = init_node();
    new_root->lhs = node;
    if ((*tokens)->type == TOKEN_AND_IF)
        new_root->type = NODE_AND_IF;
    else if ((*tokens)->type == TOKEN_OR_IF)
        new_root->type = NODE_OR_IF;
    *tokens = &((*tokens)->next);
    if(!(*tokens))
        return(print_synerr(TOKEN_END), NULL);
    new_root->rhs = parse(*tokens);
    return new_root;
}

t_node *add_paren(t_token **tokens, t_node *node)
{
    t_node *new_root;

    new_root = init_node();
    if (!new_root)
        return (perror("malloc"), NULL);
    *tokens = &((*tokens)->next);
    while((*tokens)->type && (*tokens)->type != TOKEN_CLOSE_PAREN)
    {
        if((*tokens)->type == TOKEN_SEMICOLON)
            node = add_semicolon(tokens, new_root);
        else if((*tokens)->type == TOKEN_AND_IF || (*tokens)->type == TOKEN_OR_IF || (*tokens)->type == TOKEN_PIPE)
            new_root = add_node(tokens, new_root);
        else if((*tokens)->type == TOKEN_OPEN_PAREN)
            new_root = add_paren(tokens, new_root);
        else
            new_root = add_simple(&tokens, new_root);
    }
    if(!(*tokens) || (*tokens)->type != TOKEN_CLOSE_PAREN)
        return(print_synerr(TOKEN_CLOSE_PAREN),NULL);
    *tokens = &((*tokens)->next);
    return new_root;
}

t_node *add_cmd(t_token **tokens, t_node *node)
{
    t_node *new_node;

    new_node = init_node();
    if (!new_node)
        return (perror("malloc"), NULL);
    new_node->type = NODE_CMD;
    new_node->cmd = (t_cmd *)malloc(sizeof(t_cmd));
    if(new_node->cmd == NULL)
        return (perror("malloc"), NULL);
    new_node->cmd = parse_cmd(&tokens);
    if(!node)
        return new_node;
    else
        node->rhs = new_node;
    return node;
}

t_node *parse(t_token *tokens)
{
    t_node *node;

    node = init_node();
    if (!node)
        return (perror("malloc"), NULL);
    while(tokens)
    {
        if(tokens->type == TOKEN_SEMICOLON)
            node = add_semicolon(&tokens, node);
        else if(tokens->type == TOKEN_AND_IF || tokens->type == TOKEN_OR_IF)
            node = add_condition(&tokens,node);
        else if(tokens->type == TOKEN_OPEN_PAREN)
            node = add_paren(&tokens, node);
        else if(tokens->type == TOKEN_PIPE)
            node = add_pipe(&tokens, node);
        else
            node = add_cmd(&tokens, node);
    }
    return node;
}

void free_ast(t_node *node)
{
    if (!node)
        return;
    if (node->type == NODE_CMD)
        free_tokens(node->tokens);
    free_ast(node->lhs);
    free_ast(node->rhs);
    free(node);
}
