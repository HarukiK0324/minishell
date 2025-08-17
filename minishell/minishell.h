/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:46 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/17 11:29:09 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

extern volatile sig_atomic_t	g_status;

# if defined(__APPLE__)
#  define OS 0
# else
#  define OS 1
# endif

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
	void						(*old_sigint)(int);
	void						(*old_sigquit)(int);
}								t_cmd;

typedef struct s_node
{
	t_NodeType					type;
	t_cmd						*cmd;
	void						(*old_sigint)(int);
	void						(*old_sigquit)(int);
	struct s_node				*lhs;
	struct s_node				*rhs;
}								t_node;

typedef struct s_env
{
	char						*key;
	char						*value;
	struct s_env				*next;
}								t_env;

typedef struct s_shell
{
	int							*status;
	int							*run_status;
	char						*input;
	t_env						*env_list;
	t_node						*ast;
	t_token						*tokens;
}								t_shell;

/* main.c */
int								init_shell(t_shell **shell, char **environ);
void							set_status(int *status, int value);
void							clean_up(t_shell *shell);
void							minishell(t_shell *shell);

/* builtin */
int								is_valid_identifier(char *str);
int								validate_digit_sequence(char *str, int start);
int								validate_negative_long(char *str, int i);
int								validate_positive_long(char *str, int i);
int								is_valid_long(char *str);
void							print_env_list(t_env *env_list);
int								env_no_path_error(char *cmd);
int								exec_env(t_token *argv, t_env *env_list);
long							ft_atol(const char *str);
void							exec_exit(t_shell *shell, t_token *argv);
void							print_export(t_env *env_list);
void							print_export_error(char *value);
int								handle_export_with_equals(t_token *tmp,
									t_env *env_list);
int								handle_export_without_equals(t_token *tmp,
									t_env *env_list);
int								exec_export(t_token *argv, t_env *env_list);
int								count_env(t_env *env_list);
void							swap_env(t_env **a, t_env **b);
void							sort_env_array(t_env **arr, int n);
t_env							**create_env_array(t_env *env_list, int count);
void							print_sorted_env(t_env **arr, int count);
t_env							*find_env(t_env *env_list, char *key);
void							update_existing_env(t_env *node, char *value);
void							add_new_env(t_env *env_list, char *key,
									char *value);
void							update_env(t_env *env_list, char *key,
									char *value);
char							*ft_strchr(const char *s, int c);
int								handle_cd_parent_dir(t_env *env_list);
int								handle_cd_regular(char *path, t_env *env_list);
int								handle_cd_oldpwd(t_env *env_list);
int								exec_cd(t_token *argv, t_env *env_list);
int								exec_pwd(void);
char							*find_pwd_value(t_env *env_list);
void							copy_parent_path(char *parent, char *pwd,
									int i);
char							*get_parent_from_pwd(t_env *env_list);
char							*get_cd_path(t_token *argv, t_env *env_list);
void							update_pwd(t_env *env_list, char *new_pwd);
int								only_contains(char *str, char *chars);
int								exec_echo(t_token *argv);
void							remove_env(t_env **env_list, char *key);
int								exec_unset(t_token *argv, t_env *env_list);
void							handle_append_export(t_env *env_list, char *key,
									char *value);
char							*concat_strings(const char *s1, const char *s2);

/* executor */
void							pipe_executor1(int fd[2], t_shell *shell,
									t_node *ast);
void							pipe_executor2(int fd[2], t_shell *shell,
									t_node *ast);
void							exec_pipe(t_shell *shell, t_node *ast,
									int *status);
void							exec_cmd(t_shell *shell, t_env *env_list,
									t_cmd *cmd, int *status);
void							executor(t_shell *shell, t_node *ast,
									t_env *env_list, int *status);
void							exec_builtin(t_env *env_list, t_cmd *cmd,
									int *status);
int								process_redirections(t_cmd *cmd);
void							execve_dup(t_cmd *cmd);
void							free_all_arg(char *path, char **argv,
									char **environ);
void							ft_execve(t_env *env_list, t_cmd *cmd);
void							exec_error(int *status, char *str);
int								is_builtin(char *cmd);
void							run_builtin(t_env *env_list, t_cmd *cmd,
									int *status);
void							handle_builtin_fd(t_cmd *cmd, int *status,
									int original_stdin, int original_stdout);
void							handle_status(int wstatus, int *status);
void							pipe_cleanup(int fd[], t_node *ast,
									int *status);
void							signal_pipe_hold(t_node *ast);
void							signal_pipe_revert(t_node *ast);

/* expander */
int								expand_cmd_argv(t_token *argv, t_env *env_list,
									int *status);
int								expand_cmd_fd(t_fd *fd, t_env *env_list,
									int *status);
int								expand_cmd(t_cmd *cmd, t_env *env_list,
									int *status);
int								expander(t_node *node, t_env *env_list,
									int *status);
char							*str_trim(char *str, size_t *j, size_t i);
char							*str_insert(char *str, size_t *j, char *value);
char							*replace_env_var(char *str, size_t *j, size_t i,
									char *value);
char							*replace_status(char *str, size_t *j,
									int *status);
char							*trim_quote(char *str, size_t *j, char c);
char							*trim_double_quote(char *str, size_t *j,
									t_env *env_list, int *status);
char							*parse_env_var(char *str, size_t *j,
									t_env *env_list, int *status);
char							*find_env_value(char *env_var, t_env *env_list);

/* ft_split.c */
size_t							count_words(const char *s, char c);
char							*substring(char const *s, size_t index, char c);
void							free_all(char **arr, size_t i);
char							**ft_split(char const *s, char c);

/* heredoc */
void							read_heredoc(t_fd *heredoc_delimiter, int fd);
void							parse_heredoc(t_fd *heredoc_delimiter,
									int fd_in, int fd_out);
int								ft_heredoc(t_cmd *cmd, int *run_status);
void							process_heredoc(t_cmd *cmd, int *status);
void							heredoc(t_node *ast, int *status);
void							heredoc_signal_hold(t_cmd *cmd);
void							heredoc_signal_revert(t_cmd *cmd);

/* parser */
t_node							*parse_condition(t_token **tokens, int *status);
t_node							*parse_pipe(t_token **tokens, int *status);
t_node							*create_cmd(t_token **tokens, int *status);
t_node							*parse_cmd(t_token **tokens, int *status);
t_node							*parse(t_token *tokens, int *status);
int								add_fd(t_cmd *cmd, t_token **tokens,
									int *status);
int								add_argv(t_token **argv, t_token **tokens);
int								token_cmd(t_token *tokens);

/* signal.c */
void							setup_signal_handlers(void);
void							handle_interactive_sigint(int sig);
void							reset_default_signal(void);
void							reset_heredoc_signal(void);

/* tokenizer.c */
t_token							*init_token(t_TokenType type, const char *value,
									size_t len, int *status);
t_TokenType						get_meta_type(const char *s);
size_t							add_word(const char *input, t_token **list,
									int *status);
size_t							add_metachar(const char *input, t_token **list,
									int *status);
t_token							*tokenize(const char *input, int *status);

/* utils */
int								ft_isblank(char c);
int								ismetachar(char c);
int								ft_strncmp(const char *s1, const char *s2,
									size_t n);
char							*ft_strdup(const char *s);
char							*ft_strndup(const char *s, size_t n);
void							append_token(t_token **list,
									t_token *new_token);
void							free_tokens(t_token *head);
void							free_fds(t_fd *head);
void							free_cmd(t_cmd *cmd);
void							free_node(t_node *node);
void							print_synerr(t_TokenType expected);
void							append_fd(t_fd **list, t_fd *new_fd);
t_fd							*dup_fd(t_fd *fd);
t_node							*init_node(void);
t_cmd							*init_cmd(void);
size_t							ft_strlen(const char *s);
int								ft_strcmp(const char *s1, const char *s2);
int								check_quote(char *input);
char							*append(char *s1, char *s2, char c);
void							free_env(t_env *env_list);
size_t							ft_strchar(const char *s, char c);
void							free_ast(t_node *ast);
int								is_char(char c);
int								is_numchar(char c);
char							*to_str(size_t n);
size_t							ft_token_size(t_token *tokens);
size_t							ft_env_size(t_env *env_list);
void							free_argv(char **argv, size_t i);
void							free_str_list(char **list);
char							*ft_access(char *path, char *cmd);
char							*get_path_from_env(char *argv, t_env *env_list);
char							*get_path(char *cmd, t_env *env_list);
char							**to_list(t_token *argv);
char							*ft_env_join(t_env *env_list);
char							**env_to_environ(t_env *env_list);
void							err_msg(char *msg, char *suffix);
void							err_msg_errno(char *value, char *msg);
void							ft_open_heredoc(t_cmd *cmd, int heredoc_count);
void							ft_open_fd_in(t_cmd *cmd, t_fd *current);
void							ft_open_fd_out(t_cmd *cmd, t_fd *current);
char							*get_token_str(t_TokenType token);
int								init_env(t_env **env_list, char **environ);
void							ft_exit(t_shell *shell, t_env *env_list,
									int status);
void							init_g_status(int *status);

#endif
