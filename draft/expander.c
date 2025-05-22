#include "minishell.h"

char *expand_dollar(const char *start)
{
    size_t len = 0;
    while (start[len] && ((start[len] >= 'A' && start[len] <= 'Z') ||
                          (start[len] >= 'a' && start[len] <= 'z') ||
                          (start[len] >= '0' && start[len] <= '9') ||
                          start[len] == '_'))
        len++;

    char *key = ft_strndup(start, len);
    if (!key)
        return NULL;

    char *val = getenv(key);
    free(key);

    if (!val)
        return ft_strdup("");

    return ft_strdup(val);
}

char *expand_string(const char *s)
{
    size_t i = 0;
    char *result = ft_strdup("");
    if (!result)
        return NULL;

    while (s[i])
    {
        if (s[i] == '$')
        {
            char *expanded = expand_dollar(&s[i + 1]);
            if (!expanded)
            {
                free(result);
                return NULL;
            }
            char *tmp = ft_strjoin(result, expanded);
            free(result);
            free(expanded);
            result = tmp;

            size_t skip = 0;
            while (s[i + 1 + skip] && ((s[i + 1 + skip] >= 'A' && s[i + 1 + skip] <= 'Z') ||
                                       (s[i + 1 + skip] >= 'a' && s[i + 1 + skip] <= 'z') ||
                                       (s[i + 1 + skip] >= '0' && s[i + 1 + skip] <= '9') ||
                                       s[i + 1 + skip] == '_'))
                skip++;
            i += skip + 1;
        }
        else
        {
            char buf[2] = {s[i], '\0'};
            char *tmp = ft_strjoin(result, buf);
            free(result);
            result = tmp;
            i++;
        }
    }
    return result;
}

void expand_tokens(t_token *tok)
{
    while (tok)
    {
        if (tok->type == TOKEN_DOLLAR && tok->next)
        {
            char *val = getenv(tok->next->value);
            free(tok->value);
            tok->value = ft_strdup(val ? val : "");
            tok->type = TOKEN_WORD;

            t_token *old = tok->next;
            tok->next = old->next;
            free(old->value);
            free(old);
        }
        else if (tok->type == TOKEN_DOUBLE_QUOTE)
        {
            char *expanded = expand_string(tok->value);
            if (expanded) {
                free(tok->value);
                tok->value = expanded;
            }
            tok->type = TOKEN_WORD;
        }
        else if (tok->type == TOKEN_SINGLE_QUOTE)
        {
            tok->type = TOKEN_WORD;
        }
        else if (tok->type == TOKEN_WORD && ft_strchr(tok->value, '$'))
        {
            char *expanded = expand_string(tok->value);
            if (expanded) {
                free(tok->value);
                tok->value = expanded;
            }
        }
        tok = tok->next;
    }
}

void expand_ast(t_node *node)
{
    if (!node)
        return;

    if (node->type == NODE_CMD)
        expand_tokens(node->tokens);

    expand_ast(node->lhs);
    expand_ast(node->rhs);
}
