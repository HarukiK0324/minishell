#include "minishell.h"

volatile sig_atomic_t	g_status = 0;

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

	// Set up SIGINT handler (Ctrl+C)
	sa_int.sa_handler = handle_interactive_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	// Ignore SIGQUIT (Ctrl+\)
	signal(SIGQUIT, SIG_IGN);
}

void	handle_interactive_sigint(int sig)
{
	(void)sig;
	g_status = 2;
	rl_on_new_line();
	write(STDOUT_FILENO, "\n", 1);
#if OS
	rl_replace_line("", 0);
#endif
	rl_redisplay();
}

void	reset_default_signal(void)
{
	// Reset to default behavior
	// Set up SIGINT handler (Ctrl+C)
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	reset_heredoc_signal(void)
{
	// Reset to default behavior
	// Set up SIGINT handler (Ctrl+C)
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
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
		if (g_status != 0)
		{
			status = 128 + g_status;
			g_status = 0;
		}
		if (ft_strlen(input) > 0)
		{
			if (check_quote(input) == -1)
			{
				free(input);
				err_msg("Unmatched quotes", ": Syntax error\n");
				continue ;
			}
			tokens = tokenize(input);
			ast = parse(tokens);
			if (!ast)
			{
				printf("failed tokenizing/parsing\n");
				status = 2;
			}
			else
			{
				if (expander(ast, env_list, &status) == -1)
					status = 2;
				else
					status = 0;
				heredoc(ast, &status);
				if (g_status == 0 && status == 0)
					executor(ast, env_list, &status);
			}
			free_tokens(tokens);
			free_ast(ast);
			add_history(input);
			free(input);
			g_status = 0;
		}
	}
	free_env(env_list);
	printf("exit\n");
	if (g_status != 0)
		status = 128 + g_status;
	exit(status);
}
