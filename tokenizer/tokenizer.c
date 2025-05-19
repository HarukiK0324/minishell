#include "minishell.h"

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
    while (n > 0 && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
        n--;
    }
    if (n == 0)
        return 0;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *ft_strndup(const char *string, size_t n)
{
    char    *str;
    size_t  i;

    str = (char *)malloc(sizeof(char) * (n + 1));
    if (!str)
        return (perror("ft_strndup: malloc failed"),NULL);
    i = 0;
    while (i < n && string[i])
    {
        str[i] = string[i];
        i++;
    }
    str[i] = '\0';
    return str;
}

size_t ft_strlen(const char *s)
{
    size_t i;

    i = 0;
    while (s[i])
        i++;
    return i;
}


int ft_isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

int ismetachar(char c)
{
    return (c == '|' || c == '<' || c == '>' || c == '(' || c == ')' || c == ';' ||
            c == '&' || c == '$' || c == '`' || c == '\\' || c == '=');
}

void append_token(Token **token_list, Token *new_token)
{
    if (!new_token)
        return;
    if (!(*token_list))
    {
        *token_list = new_token;
    }
    else
    {
        Token *temp = *token_list;
        while (temp->next)
            temp = temp->next;
        temp->next = new_token;
    }
}

int add_word(const char *string, Token **token_list)
{
    Token   *new_token;
    size_t  len;

    len = 0;
    while (string[len] && !ismetachar(string[len]) && !ft_isspace(string[len]) &&
           string[len] != '\'' && string[len] != '\"' )
        len++;
    new_token = (Token *)malloc(sizeof(Token));
    if (!new_token)
        return(perror("add_word: malloc for token failed"),0);
    new_token->value = ft_strndup(string, len);
    if (!new_token->value)
    {
        free(new_token);
        return (perror("add_word: ft_strndup failed"),0);
    }
    new_token->type = TOKEN_WORD;
    new_token->next = NULL;
    append_token(token_list, new_token);
    return len;
}

TokenType get_meta_type(const char *string)
{
    if (ft_strncmp(string, "<<", 2) == 0) return TOKEN_HEREDOC;
    if (ft_strncmp(string, ">>", 2) == 0) return TOKEN_APPEND;
    if (ft_strncmp(string, "&&", 2) == 0) return TOKEN_AND_IF;
    if (ft_strncmp(string, "||", 2) == 0) return TOKEN_OR_IF;
    if (ft_strncmp(string, "|", 1) == 0) return TOKEN_PIPE;
    if (ft_strncmp(string, "<", 1) == 0) return TOKEN_REDIR_IN;
    if (ft_strncmp(string, ">", 1) == 0) return TOKEN_REDIR_OUT;
    if (ft_strncmp(string, "(", 1) == 0) return TOKEN_OPEN_PAREN;
    if (ft_strncmp(string, ")", 1) == 0) return TOKEN_CLOSE_PAREN;
    if (ft_strncmp(string, ";", 1) == 0) return TOKEN_SEMICOLON;
    if (ft_strncmp(string, "&", 1) == 0) return TOKEN_AMPERSAND;
    if (ft_strncmp(string, "`", 1) == 0) return TOKEN_BACKTICK;
    if (ft_strncmp(string, "$", 1) == 0) return TOKEN_DOLLAR;
    if (ft_strncmp(string, "\\", 1) == 0) return TOKEN_BACKSLASH;
    if (ft_strncmp(string, "=", 1) == 0) return TOKEN_EQUALS;
    return TOKEN_WORD;
}

int add_metachar(const char *string, Token **token_list)
{
    Token       *new_token;
    TokenType   type;
    int         len;

    type = get_meta_type(string);
    if (type == TOKEN_HEREDOC || type == TOKEN_APPEND ||
        type == TOKEN_AND_IF || type == TOKEN_OR_IF)
        len = 2;
    else if (type != TOKEN_WORD)
        len = 1;
    else
        return 0;
    new_token = (Token *)malloc(sizeof(Token));
    if (!new_token)
        return (perror("add_metachar: malloc for token failed"),0);
    new_token->value = ft_strndup(string, len);
    if (!new_token->value)
    {
        free(new_token);
        return (perror("add_metachar: ft_strndup failed"),0);
    }
    new_token->type = type;
    new_token->next = NULL;
    append_token(token_list, new_token);
    return len;
}

int add_quote(const char *string, Token **token_list, char quote_char)
{
    Token   *new_token;
    size_t  len;
    const char *start_of_content;

    start_of_content = string + 1;
    len = 0;
    while (start_of_content[len] != '\0' && start_of_content[len] != quote_char)
        len++;
    new_token = (Token *)malloc(sizeof(Token));
    if (!new_token)
        return (perror("add_quote: malloc for token failed"),0);
    new_token->value = ft_strndup(start_of_content, len);
    if (!new_token->value)
    {
        free(new_token);
        return (perror("add_quote: ft_strndup failed"),0);
    }
    new_token->type = (quote_char == '"') ? TOKEN_DOUBLE_QUOTE : TOKEN_SINGLE_QUOTE;
    new_token->next = NULL;
    append_token(token_list, new_token);
    return len + 2;
}

Token *tokenize(const char *input)
{
    Token   *token_head;
    int     consumed_len;

    token_head = NULL;
    while (*input)
    {
        while (*input && ft_isspace((unsigned char)*input))
            input++;
        if (*input == '\0')
            break;
        consumed_len = 0;
        if (*input == '\'' || *input == '\"')
            consumed_len = add_quote(input, &token_head, *input);
        else if (ismetachar(*input))
            consumed_len = add_metachar(input, &token_head);
        else
            consumed_len = add_word(input, &token_head);
        if (consumed_len <= 0)
            return (free_tokens(token_head),NULL);
        input += consumed_len;
    }
    return token_head;
}

void free_tokens(Token *token_list)
{
    Token *current;
    Token *next_token;

    current = token_list;
    while (current != NULL)
    {
        next_token = current->next;
        free(current->value);
        free(current);
        current = next_token;
    }
}
