#include "minishell.h"

volatile sig_atomic_t	g_status = 0;

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	if (s == NULL)
		return (0);
	while (s[i] != '\0')
		i++;
	return (i);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(const unsigned char *)s1 - *(const unsigned char *)s2);
}

int	check_quote(char *input)
{
	while (*input != '\0')
	{
		if (*input == '\"')
		{
			input++;
			while (*input != '\0' && *input != '\"')
				input++;
			if (*input == '\0')
				return (-1);
		}
		else if (*input == '\'')
		{
			input++;
			while (*input != '\0' && *input != '\'')
				input++;
			if (*input == '\0')
				return (-1);
		}
		input++;
	}
	return (1);
}

char	*append(char *s1, char *s2, char c)
{
	char	*result;
	size_t	len1;
	size_t	len2;
	int		i;
	int		j;

	if (!s1 || !s2)
		return (perror("append failed"), NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	result = (char *)malloc(len1 + len2 + 2);
	if (!result)
		return (perror("malloc failed"), NULL);
	i = -1;
	while (s1 && s1[++i] != '\0')
		result[i] = s1[i];
	result[i++] = c;
	j = -1;
	while (s2 && s2[++j] != '\0')
		result[i + j] = s2[j];
	result[i + j] = '\0';
	free(s1);
	free(s2);
	return (result);
}

void	free_env(t_env *env_list)
{
	t_env	*temp;

	while (env_list)
	{
		temp = env_list;
		env_list = env_list->next;
		free(temp->key);
		free(temp->value);
		free(temp);
	}
}

size_t	ft_strchar(const char *s, char c)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			return (i);
		i++;
	}
	return (i);
}

t_env	*init_env(char **environ)
{
	t_env	*env_list;
	t_env	*new_node;
	int		i;

	env_list = NULL;
	i = 0;
	while (environ[i])
	{
		new_node = (t_env *)malloc(sizeof(t_env));
		if (!new_node)
			return (perror("malloc"), free_env(env_list), NULL);
		new_node->key = ft_strndup(environ[i], ft_strchar(environ[i], '='));
		new_node->value = ft_strdup(environ[i] + ft_strchar(environ[i], '=')
				+ 1);
		new_node->next = env_list;
		env_list = new_node;
		i++;
	}
	return (env_list);
}

void	setup_signal_handlers(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// Set up SIGINT handler (Ctrl+C)
	sa_int.sa_handler = handle_interactive_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	// Ignore SIGQUIT (Ctrl+\)
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

void	handle_interactive_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
#if HAVE_RL_REPLACE_LINE
    rl_replace_line("", 0);
#endif
	rl_redisplay();
	errno = EINTR;
}

void	handle_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
#if HAVE_RL_REPLACE_LINE
    rl_replace_line("", 0);
#endif
	rl_redisplay();
	g_status = 2;
	errno = EINTR;
}

void	handle_sigquit(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
	rl_on_new_line();
#if HAVE_RL_REPLACE_LINE
    rl_replace_line("", 0);
#endif
	rl_redisplay();
	g_status = 3;
	errno = EINTR;
}

void	reset_default_signal(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// Reset to default behavior
	// Set up SIGINT handler (Ctrl+C)
	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	sa_quit.sa_handler = handle_sigquit;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

void	reset_heredoc_signal(void)
{
	struct sigaction	sa_int;

	// Reset to default behavior
	// Set up SIGINT handler (Ctrl+C)
	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	signal(SIGQUIT, SIG_DFL);
}

void	free_ast(t_node *ast)
{
	if (!ast)
		return ;
	if (ast->cmd)
		free_cmd(ast->cmd);
	if (ast->lhs)
		free_ast(ast->lhs);
	if (ast->rhs)
		free_ast(ast->rhs);
	free(ast);
}

void	print_ast(t_node *ast)
{
	t_token	*arg;
	t_fd	*fd;
	t_fd	*heredoc;

	if (!ast)
		return ;
	switch (ast->type)
	{
	case NODE_CMD:
		printf("Command Node:\n");
		if (ast->cmd)
		{
			arg = ast->cmd->argv;
			while (arg)
			{
				printf("  Arg: %s\n", arg->value);
				arg = arg->next;
			}
			fd = ast->cmd->fds;
			while (fd)
			{
				printf("  FD: %s (type: ", fd->value);
				switch (fd->type)
				{
				case TOKEN_REDIR_IN:
					printf("Redirect In)\n");
					break ;
				case TOKEN_REDIR_OUT:
					printf("Redirect Out)\n");
					break ;
				case TOKEN_HEREDOC:
					printf("Heredoc)\n");
					break ;
				case TOKEN_APPEND:
					printf("Append)\n");
					break ;
				default:
					printf("Unknown)\n");
					break ;
				}
				fd = fd->next;
			}
			heredoc = ast->cmd->heredoc_delimiter;
			while (heredoc)
			{
				printf("  Heredoc Delimiter: %s\n", heredoc->value);
				heredoc = heredoc->next;
			}
		}
		break ;
	case NODE_PIPE:
		printf("Pipe Node:\n");
		break ;
	case NODE_AND_IF:
		printf("And If Node:\n");
		break ;
	case NODE_OR_IF:
		printf("Or If Node:\n");
		break ;
	case NODE_INIT:
		printf("Init Node:\n");
		break ;
	default:
		printf("Unknown Node Type\n");
		break ;
	}
	print_ast(ast->lhs);
	print_ast(ast->rhs);
}

int	main(int argc, char **argv, char **environ)
{
	char	*input;
	t_token	*tokens;
	t_node	*ast;
	t_env	*env_list;
	int		status;
	t_token	*saved_tokens;

	(void)argc; // Unused parameter
	(void)argv; // Unused parameter
	status = 0;
	env_list = init_env(environ);
	if (!env_list)
		return (perror("init_env failed"), 1);
	// Exit if environment initialization fails
	while (1)
	{
		setup_signal_handlers();
		// Set up signal handlers for Ctrl+C and Ctrl+'\'
		input = readline("minishell$ ");
		if (!input)
			break ; // Exit on EOF (Ctrl+D)
		if (ft_strlen(input) > 0)
		{
			if (check_quote(input) == -1)
			{
				free(input);
				err_msg("Unmatched quotes", ": Syntax error\n");
				continue ;
			}
			tokens = tokenize(input);
			saved_tokens = tokens;
			// while (saved_tokens)
			// {
			// 	if (!saved_tokens->type)
			// 	{
			// 		printf("Token type is not set.\n");
			// 		break ;
			// 	}
			// 	if (saved_tokens->type == TOKEN_WORD)
			// 		printf("Word:");
			// 	else if (saved_tokens->type == TOKEN_PIPE)
			// 		printf("Pipe:");
			// 	else if (saved_tokens->type == TOKEN_REDIR_IN)
			// 		printf("Redirect In:");
			// 	else if (saved_tokens->type == TOKEN_REDIR_OUT)
			// 		printf("Redirect Out:");
			// 	else if (saved_tokens->type == TOKEN_HEREDOC)
			// 		printf("Heredoc:");
			// 	else if (saved_tokens->type == TOKEN_APPEND)
			// 		printf("Append:");
			// 	else if (saved_tokens->type == TOKEN_AND_IF)
			// 		printf("And If:");
			// 	else if (saved_tokens->type == TOKEN_OR_IF)
			// 		printf("Or If:");
			// 	else if (saved_tokens->type == TOKEN_OPEN_PAREN)
			// 		printf("Open Parenthesis:");
			// 	else if (saved_tokens->type == TOKEN_CLOSE_PAREN)
			// 		printf("Close Parenthesis:");
			// 	else if (saved_tokens->type == TOKEN_END)
			// 		printf("End of Tokens:");
			// 	else if (saved_tokens->type == TOKEN_NEWLINE)
			// 		printf("Newline:");
			// 	else if (saved_tokens->type == TOKEN_ERROR)
			// 		printf("Error:");
			// 	else
			// 		printf("Unknown Token:");
			// 	if (!saved_tokens->value)
			// 		printf(" (No value)\n");
			// 	else
			// 		printf(" Token: %s\n", saved_tokens->value);
			// 	saved_tokens = saved_tokens->next;
			// }
			ast = parse(tokens);
			if (!ast)
			{
				printf("failed tokenizing/parsing\n");
				status = 2;
			}
			expander(ast, env_list, &status);
			if (g_status != 0)
				status = 2;
			else
				status = 0;
			heredoc(ast, &status);
			if (g_status == 0 && status == 0)
				executor(ast, env_list, &status);
			free_tokens(tokens);
			free_ast(ast);
			add_history(input);
			free(input);
			g_status = 0;
		}
	}
	free_env(env_list);
	printf("exit\n");
	exit(status);
}
