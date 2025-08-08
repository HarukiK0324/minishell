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
