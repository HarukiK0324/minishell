#include "minishell.h"

extern volatile sig_atomic_t g_status = 0;
extern char **environ;

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
				return -1;
		}
		else if (*input == '\'')
		{
			input++;
			while (*input != '\0' && *input != '\'')
				input++;
			if (*input == '\0')
				return -1;
		}
		input++;
	}
	return (1);
}

char *append(char *s1, char *s2,char c)
{
	char *result;
	size_t len1;
	size_t len2;
	int i;
	int j;

	if(!s1 || !s2)
		return (perror("append failed"), NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	result = (char *)malloc(len1 + len2 + 2);
	if (!result)
		return (perror("malloc failed"), NULL);
	i = -1;
	while(s1 && s1[++i] != '\0')
		result[i] = s1[i];
	result[i++] = c;
	j = -1;
	while(s2 && s2[++j] != '\0')
		result[i + j] = s2[j];
	result[i + j] = '\0';
	free(s1);
	free(s2);
	return result;
}

int	main(void)
{
	char	*input;
	t_token	*tokens;
	t_node	*ast;
	int status;

	// signal(SIGINT, );
	while (1)
	{
		input = readline("minishell$ ");
		if (input && ft_strlen(input) > 0)
		{
			while(check_quote(input) == -1)
				input = append(input,readline("> "),'\n');
			// tokens = tokenize(input);
			// ast = parse(tokens);
			// if(!ast)
			// 	//exit status 2 or ???
			// status = expander(ast);
			// status = executor(ast);
			add_history(input);
			free(input);
		}
	}
	printf("exit\n");
}
