#include "minishell.h"

char *ft_strncmp(const char *s1, const char *s2, int n)
{
    while (n && *s1 && *s2 && (*s1 == *s2))
    {
        s1++;
        s2++;
        n--;
    }
    if (n == 0)
        return 0;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *ft_strndup(char *string,int n)
{
    char *str;
    int i;

    str = (char *)malloc(sizeof(char) * (n + 1));
    if (!str)
        return NULL;
    i = 0;
    while (i < n)
    {
        str[i] = string[i];
        i++;
    }
    str[i] = '\0';
    return str;
}

int ft_isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

int ismetachar(char c)
{
    return (c == '|' || c == '<' || c == '>' || c == '(' || c == ')' || c == ';' ||
            c == '&' || c == '$' || c == '`' || c == '\'' || c == '"' || c == '\\' ||
            c == '=');
}

int add_word(char *string, Token **token)
{
    Token *new_token;
    int len;

    new_token = (Token *)malloc(sizeof(Token));
    if (!new_token)
        return;
    len = 0;
    while(string[len] != '\0' && !ismetachar(string[len]) && !ft_isspace(string[len]))
        len++;
    new_token->value = ft_strndup(string, len);
    new_token->type = TOKEN_WORD;
    new_token->next = NULL;
    if(!(*token))
        *token = new_token;
    else
    {
        Token *temp = *token;
        while (temp->next)
            temp = temp->next;
        temp->next = new_token;
    }
    return len;
}

char *get_meta_type(char *string)
{
    if(ft_strncmp(string, "<<", 2) == 0)
        return TOKEN_HEREDOC;
    else if(ft_strncmp(string, ">>", 2) == 0)
        return TOKEN_APPEND;
    else if(ft_strncmp(string, "&&", 2) == 0)
        return TOKEN_AND_IF;
    else if(ft_strncmp(string, "||", 2) == 0)
        return TOKEN_OR_IF;
    else if(ft_strncmp(string, "|", 1) == 0)
        return TOKEN_PIPE;
    else if(ft_strncmp(string, "<", 1) == 0)
        return TOKEN_REDIR_IN;
    else if(ft_strncmp(string, ">", 1) == 0)
        return TOKEN_REDIR_OUT;
    else if(ft_strncmp(string, "(", 1) == 0)
        return TOKEN_OPEN_PAREN;
    else if(ft_strncmp(string, ")", 1) == 0)
        return TOKEN_CLOSE_PAREN;
    else if(ft_strncmp(string, ";", 1) == 0)
        return TOKEN_SEMICOLON;
    else if(ft_strncmp(string, "&", 1) == 0)
        return TOKEN_AMPERSAND;
    else if(ft_strncmp(string, "`", 1) == 0)
        return TOKEN_BACKTICK;
    else if(ft_strncmp(string, "$", 1) == 0)
        return TOKEN_DOLLAR;
    else if(ft_strncmp(string, "\\", 1) == 0)
        return TOKEN_BACKSLASH;
    else if(ft_strncmp(string, "=", 1) == 0)
        return TOKEN_EQUALS;
}

int add_metachar(char *string, Token **token)
{
    Token *new_token;

    new_token = (Token *)malloc(sizeof(Token));
    if (!new_token)
        return;
    new_token->value = NULL;
    new_token->type = get_meta_type(new_token->value);
    new_token->next = NULL;
    if(!(*token))
        *token = new_token;
    else
    {
        Token *temp = *token;
        while (temp->next)
            temp = temp->next;
        temp->next = new_token;
    }
    return ft_strlen(new_token->value);
}

int add_quote(char *string, Token **token,char quote)
{
    Token *new_token;
    int len;

    new_token = (Token *)malloc(sizeof(Token));
    if (!new_token)
        return;
    len = 0;
    string++;
    while(string[len] != '\0' && string[len] != quote)
        len++;
    new_token->value = ft_strndup(string, len);
    if(quote == '"')
        new_token->type = TOKEN_DOUBLE_QUOTE;
    else
        new_token->type = TOKEN_SINGLE_QUOTE;
    new_token->next = NULL;
    if(!(*token))
        *token = new_token;
    else
    {
        Token *temp = *token;
        while (temp->next)
            temp = temp->next;
        temp->next = new_token;
    }
    return len + 2;
}

Token *tokenize(char *input)
{
    Token *token;

    token = NULL;
    while(*input != '\0')
    {
        while(*input && ft_isspace((int)*input))
            input++;
        if(*input == '\0')
            break;
        else if(*input == '\"')
            input += add_quote(input, &token, '\"');
        else if (*input == '\'')
            input += add_quote(input, &token,'\'');
        else if(ismetachar(*input))
            input += add_metachar(input, &token);
        else
            input += add_word(input, &token);
    }
    return token;
}
