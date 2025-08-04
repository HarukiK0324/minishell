#ifndef MINISHELL_H
# define MINISHELL_H

# define _XOPEN_SOURCE 700
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

extern volatile sig_atomic_t	g_status;

typedef enum t_TokenType
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
}								t_TokenType;

typedef enum t_NodeType
{
	NODE_PIPE,
	NODE_CMD,
	NODE_AND_IF,
	NODE_OR_IF,
	NODE_INIT
}								t_NodeType;

typedef struct s_token
{
	t_TokenType					type;
	char						*value;
	struct s_token				*next;
}								t_token;

typedef struct s_fd
{
	t_TokenType					type;
	int							fd;
	char						*value;
	struct s_fd					*next;
}								t_fd;

typedef struct s_cmd
{
	t_token						*argv;
	t_fd						*fds;
	t_fd						*heredoc_delimiter;
	int							heredoc_count;
	int							heredoc_fd;
	int							fd_in;
	int							fd_out;
}								t_cmd;

typedef struct s_node
{
	t_NodeType					type;
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
size_t							ft_strlen(const char *s);
int								ft_strcmp(const char *s1, const char *s2);
int								check_quote(char *input);
char							*append(char *s1, char *s2, char c);
void							free_list(t_env *env_list);
size_t							ft_strchar(const char *s, char c);
t_env							*init_env(char **environ);
void							setup_signal_handlers(void);
void							handle_sigint(int sig);
void							handle_interactive_sigint(int sig);
void							reset_default_signal(void);
void							reset_heredoc_signal(void);

/* builtin.c */
int								only_contains(char *str, char *chars);
int								exec_echo(t_token *argv);
int								exec_cd(t_token *argv, t_env *env_list);
int								exec_pwd(void);
int								exec_export(t_token *argv, t_env *env_list);
int								exec_unset(t_token *argv, t_env *env_list);
int								exec_env(t_env *env_list);
int								exec_exit(t_token *argv);

/* executor.c */
int								is_builtin(char *cmd);
void							exec_builtin(t_env *env_list, t_cmd *cmd,
									int *status);
void 							free_str_list(char **list);
char							*ft_access(char *path, char *cmd);
char							*get_path_from_env(char *argv, t_env *env_list);
char							*get_path(char *cmd, t_env *env_list);
size_t							ft_token_size(t_token *tokens);
size_t							ft_env_size(t_env *env_list);
void							free_argv(char **argv, size_t i);
char							**to_list(t_token *tokens);
char							*ft_env_join(t_env *env_list);
char							**env_to_environ(t_env *env_list);
void 							read_heredoc(t_fd *heredoc_delimiter, int fd);
void 							parse_heredoc(t_fd *heredoc_delimiter, int fd_in, int fd_out);
int 							ft_heredoc(t_cmd *cmd);
void 							err_msg(char *value, char *msg);
void 							ft_open_heredoc(t_cmd *cmd, t_fd *current, int heredoc_count);
void 							ft_open_fd_in(t_cmd *cmd, t_fd *current);
void 							ft_open_fd_out(t_cmd *cmd, t_fd *current);
int 							process_redirections(t_cmd *cmd);
void							executor(t_node *ast, t_env *env_list,
									int *status);
void							exec_pipe(t_node *ast, t_env *env_list,
									int *status);
void							exec_cmd(t_env *env_list, t_cmd *cmd,
									int *status);
void							ft_execve(t_env *env_list, t_cmd *cmd,
									int *status);

/* expander.c */
int								is_char(char c);
int								is_numchar(char c);
char							*to_str(size_t n);
char							*str_trim(char *str, size_t *j, size_t i);
char							*str_insert(char *str, size_t *j, char *value);
char							*replace_env_var(char *str, size_t *j, size_t i,
									char *env_var, t_env *env_list);
char							*replace_status(char *str, size_t *j, int *status);
char							*parse_env_var(char *str, size_t *j,
									t_env *env_list, int *status);
char							*trim_quote(char *str, size_t *j, char c);
char							*trim_double_quote(char *str, size_t *j,
									t_env *env_list, int *status);
void							expand_cmd(t_cmd *cmd, t_env *env_list,
									int *status);
void							expander(t_node *node, t_env *env_list,
									int *status);

/* ft_split.c */
size_t							count_words(const char *s, char c);
char							*substring(char const *s, size_t index, char c);
void							free_all(char **arr, size_t i);
char							**ft_split(char const *s, char c);

/* heredoc.c */
void							read_heredoc(t_fd *heredoc_delimiter, int fd);
void							parse_heredoc(t_fd *heredoc_delimiter, int fd_in, int fd_out);
int								ft_heredoc(t_cmd *cmd);
void							process_heredoc(t_cmd *cmd, int *status);
void							heredoc(t_node *ast, int *status);

/* parser.c */
void							print_synerr(t_TokenType expected);
void							free_cmd(t_cmd *cmd);
void							free_node(t_node *node);
int								token_cmd(t_token *tokens);
t_node							*init_node(void);
t_cmd							*init_cmd(void);
t_node							*parse_condition(t_token **tokens);
t_node							*parse_pipe(t_token **tokens);
int								add_fd(t_cmd *cmd, t_token **tokens);
int								add_argv(t_token **argv, t_token **tokens);
t_node							*parse_cmd(t_token **tokens);
t_node							*add_condition(t_token **tokens, t_node *node);
t_node							*add_paren(t_token **tokens, t_node *node);
t_node							*add_pipe(t_token **tokens, t_node *node);
t_node							*add_cmd(t_token **tokens, t_node *node);
t_node							*parse(t_token *tokens);

/* tokenizer.c */
int								ft_isblank(char c);
int								ismetachar(char c);
int								ft_strncmp(const char *s1, const char *s2,
									size_t n);
char							*ft_strdup(const char *s);
char							*ft_strndup(const char *s, size_t n);
void							append_token(t_token **list,
									t_token *new_token);
t_TokenType						get_meta_type(const char *s);
size_t							add_word(const char *input, t_token **list);
size_t							add_metachar(const char *input, t_token **list);
void							free_tokens(t_token *head);
void							free_fds(t_fd *head);
t_token							*tokenize(const char *input);

#endif
