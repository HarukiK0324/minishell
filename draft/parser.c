#include "minishell.h"

t_node *new_node(NodeType type, t_node *lhs, t_node *rhs, t_token *tokens)
{
    t_node *node = (t_node *)malloc(sizeof(t_node));
    if (!node)
        return NULL;
    node->type = type;
    node->lhs = lhs;
    node->rhs = rhs;
    node->tokens = tokens;
    return node;
}

t_token *consume_token(t_token **tokens)
{
    t_token *t = *tokens;
    if (t)
        *tokens = t->next;
    return t;
}

t_node *parse_simple(t_token **tokens)
{
    if (!*tokens)
        return NULL;

    if ((*tokens)->type == TOKEN_OPEN_PAREN)
    {
        consume_token(tokens);
        t_node *inner = parse(*tokens);
        if (*tokens && (*tokens)->type == TOKEN_CLOSE_PAREN)
            consume_token(tokens);
        return inner;
    }

    t_token *head = NULL, *tail = NULL;
    while (*tokens && (*tokens)->type != TOKEN_PIPE &&
           (*tokens)->type != TOKEN_AND_IF &&
           (*tokens)->type != TOKEN_OR_IF &&
           (*tokens)->type != TOKEN_SEMICOLON &&
           (*tokens)->type != TOKEN_CLOSE_PAREN)
    {
        t_token *t = consume_token(tokens);
        t->next = NULL;
        if (!head)
            head = tail = t;
        else
        {
            tail->next = t;
            tail = t;
        }
    }

    if (!head)
        return NULL;

    return new_node(NODE_CMD, NULL, NULL, head);
}

t_node *parse_pipe(t_token **tokens)
{
    t_node *left = parse_simple(tokens);
    if (!left)
        return NULL;

    while (*tokens && (*tokens)->type == TOKEN_PIPE)
    {
        consume_token(tokens);
        t_node *right = parse_simple(tokens);
        left = new_node(NODE_PIPE, left, right, NULL);
    }

    return left;
}

t_node *parse_and_or(t_token **tokens)
{
    t_node *left = parse_pipe(tokens);
    if (!left)
        return NULL;

    while (*tokens && ((*tokens)->type == TOKEN_AND_IF || (*tokens)->type == TOKEN_OR_IF))
    {
        TokenType op = (*tokens)->type;
        consume_token(tokens);
        t_node *right = parse_pipe(tokens);
        left = new_node(op == TOKEN_AND_IF ? NODE_AND_IF : NODE_OR_IF, left, right, NULL);
    }

    return left;
}

t_node *parse_semicolon(t_token **tokens)
{
    t_node *left = parse_and_or(tokens);
    if (!left)
        return NULL;

    while (*tokens && (*tokens)->type == TOKEN_SEMICOLON)
    {
        consume_token(tokens);
        t_node *right = parse_and_or(tokens);
        left = new_node(NODE_SEMICOLON, left, right, NULL);
    }

    return left;
}

t_node *parse(t_token *token_list)
{
    return parse_semicolon(&token_list);
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
