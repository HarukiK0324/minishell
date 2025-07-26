#include "minishell.h"

extern volatile sig_atomic_t g_status;

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

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
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

void free_env(t_env *env_list)
{
	t_env *temp;

	while (env_list)
	{
		temp = env_list;
		env_list = env_list->next;
		free(temp->key);
		free(temp->value);
		free(temp);
	}
}

size_t ft_strchar(const char *s, char c)
{
	size_t i;

	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			return (i);
		i++;
	}
	return (i);
}

void sort_env_list(t_env **env_list)
{
	t_env *end = NULL;
	int swapped;

	if (!env_list || !*env_list)
		return;
	swapped = 1;
	while (swapped)
	{
		t_env *cur = *env_list;
		swapped = 0;
		while (cur->next != end)
		{
			if (ft_strcmp(cur->key, cur->next->key) > 0)
			{
				char *k = cur->key, *v = cur->value;
				cur->key = cur->next->key;
				cur->value = cur->next->value;
				cur->next->key = k;
				cur->next->value = v;
				swapped = 1;
			}
			cur = cur->next;
		}
		end = cur;
	}
}

t_env *init_env(char **environ)
{
	t_env *env_list;
	t_env *new_node;
	int i;

	env_list = NULL;
	i = 0;
	while (environ[i])
	{
		new_node = (t_env *)malloc(sizeof(t_env));
		if (!new_node)
			return (perror("malloc"), free_env(env_list), NULL);
		new_node->key = ft_strndup(environ[i],ft_strchar(environ[i], '='));
		new_node->value = ft_strdup(environ[i] + ft_strchar(environ[i], '=') + 1);
		new_node->next = env_list;
		env_list = new_node;
		i++;
	}
	return env_list;
}

int	main(int argc, char **argv, char **environ)
{
	char	*input;
	t_token	*tokens;
	t_node	*ast;
	t_env	*env_list;
	int status;

	// signal(SIGINT, );
	(void)argc; // Unused parameter
	(void)argv; // Unused parameter
	env_list = init_env(environ);
	while(env_list)
    {
        printf("%s=%s\n", env_list->key, env_list->value);
        env_list = env_list->next;
    }
	if(!env_list)
		return (perror("init_env failed"), 1); // Exit if environment initialization fails
	while (1)
	{
		input = readline("minishell$ ");
		if (input && ft_strlen(input) > 0)
		{
			while(check_quote(input) == -1)
				input = append(input,readline("> "),'\n');
			tokens = tokenize(input);
			ast = parse(tokens);
			if(!ast)
				status = 2;
			expander(ast,env_list,&status);
			executor(ast,env_list,&status);
			free_ast(ast);
			add_history(input);
			free(input);
		}
	}
	printf("exit\n");
}
