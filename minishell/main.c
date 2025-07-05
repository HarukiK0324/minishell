#include "minishell.h"

volatile sig_atomic_t	g_status = 0;

size_t	ft_strlen(char *s)
{
	size_t	i;

	i = 0;
	if (s == NULL)
		return (0);
	while (s[i] != '\0')
		i++;
	return (i);
}
int	check_input(char *input)
{
	while (*input != '\0')
	{
		if (*input == ';' || *input == '\\')
			return (printf("Error\n"), 0);
		else if (*input == '\"')
		{
			input++;
			while (*input != '\0' && *input != '\"')
				input++;
			if (*input == '\0')
				return (printf("Error\n"), 0);
		}
		else if (*input == '\'')
		{
			input++;
			while (*input != '\0' && *input != '\'')
				input++;
			if (*input == '\0')
				return (printf("Error\n"), 0);
		}
		input++;
	}
	return (1);
}

int	main(void)
{
	char	*input;
	t_token	*tokens;
	t_node	*ast;

	// signal(SIGINT, );
	while (1)
	{
		input = readline("minishell$ ");
		if (input == NULL || ft_strlen(input) == 0 || check_input(input) == 0)
		{
			free(input);
			continue ;
		}
		tokens = tokenize(input);
		ast = parse(tokens);
		// executor(input);
		add_history(input);
		free(input);
	}
	printf("exit\n");
}
