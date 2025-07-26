/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:23:34 by hkasamat          #+#    #+#             */
/*   Updated: 2025/07/26 16:42:43 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
#include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef enum
{
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_HEREDOC,
	TOKEN_APPEND,
	TOKEN_OPEN_PAREN,
	TOKEN_CLOSE_PAREN,
	TOKEN_AND_IF,
	TOKEN_OR_IF,
	TOKEN_WORD,
	TOKEN_END,
	TOKEN_NEWLINE,
	TOKEN_ERROR
}								TokenType;

typedef enum
{
	NODE_PIPE,
	NODE_CMD,
	NODE_AND_IF,
	NODE_OR_IF,
	NODE_INIT
}								NodeType;

typedef struct s_token
{
	TokenType					type;
	char						*value;
	struct s_token				*next;
}								t_token;

typedef struct s_fd
{
	TokenType					type;
	int 						fd;
	char						*value;
	struct s_fd					*next;
}								t_fd;

typedef struct s_cmd
{
	t_token						*argv;
	t_fd						*fds;
	t_fd						*heredoc_delimiter;
}								t_cmd;

typedef struct s_node
{
	NodeType					type;
	t_cmd						*cmd;
	struct s_node				*lhs;
	struct s_node				*rhs;
}								t_node;

typedef struct s_env
{
	char						*key;
	char						*value;
	struct s_env				*next;
}								t_env;

/* main.c */
size_t      ft_strlen(const char *s);
int         ft_strcmp(const char *s1, const char *s2);
int         check_quote(char *input);
char *append(char *s1, char *s2,char c);
void free_list(t_env *env_list);
size_t ft_strchar(const char *s, char c);
void sort_env_list(t_env **env_list);
t_env      *init_env(char **environ);

/* executor.c */
int         is_builtin(char *cmd);
int         exec_builtin(t_env *env_list, t_cmd *cmd);
void        executor(t_node *ast, t_env *env_list, int *status);
void        exec_pipe(t_node *ast, t_env *env_list, int *status);
void        exec_cmd(t_env *env_list, t_cmd *cmd, int *status);
void        ft_execve(t_env *env_list, t_cmd *cmd, int *status);

/* expander.c */
int         is_char(char c);
int         is_numchar(char c);
char       *to_str(int n);
char       *str_trim(char *str, int *j, int i);
char       *str_insert(char *str, int *j, char *value);
char       *replace_env_var(char *str, int *j, int i, char *env_var, t_env *env_list);
char       *replace_status(char *str, int *j, int *status);
char       *parse_env_var(char *str, int *j, t_env *env_list, int *status);
char       *trim_quote(char *str, int *j, char c);
char       *trim_double_quote(char *str, int *j, t_env *env_list, int *status);
void        expand_cmd(t_cmd *cmd, t_env *env_list, int *status);
void        expander(t_node *node, t_env *env_list, int *status);

/* parser.c */
void        print_synerr(TokenType expected);
void        free_cmd(t_cmd *cmd);
void        free_node(t_node *node);
int         token_cmd(t_token *tokens);
t_node     *init_node(void);
t_cmd      *init_cmd(void);
t_node     *parse_condition(t_token **tokens);
t_node     *parse_pipe(t_token **tokens);
int add_fd(t_cmd *cmd, t_token **tokens);
int add_argv(t_token *argv, t_token **tokens);
t_cmd *parse_cmd(t_token **tokens);
t_node *add_condition(t_token **tokens, t_node *node);
t_node *add_paren(t_token **tokens, t_node *node);
t_node *add_pipe(t_token **tokens, t_node *node);
t_node     *add_cmd(t_token **tokens, t_node *node);
t_node     *parse(t_token *tokens);

/* tokenizer.c */
int         ft_isblank(char c);
int         ismetachar(char c);
int         ft_strncmp(const char *s1, const char *s2, size_t n);
char       *ft_strdup(const char *s);
char       *ft_strndup(const char *s, size_t n);
void        append_token(t_token **list, t_token *new_token);
TokenType   get_meta_type(const char *s);
size_t      add_word(const char *input, t_token **list);
size_t      add_metachar(const char *input, t_token **list);
void        free_tokens(t_token *head);
t_token    *tokenize(const char *input);

#endif
