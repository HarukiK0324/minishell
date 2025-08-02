#include "minishell.h"

int	only_contains(char *str, char *chars)
{
	int	i;
	int	j;

	i = 1;
	while (str[i] != '\0')
	{
		j = 0;
		while (chars[j] != '\0')
		{
			if (str[i] == chars[j])
				break ;
			j++;
		}
		if (chars[j] == '\0')
			return (0);
		i++;
	}
	return (1);
}

int	exec_echo(t_token *argv)
{
	int		newline;
	t_token	*tmp;

	newline = 1;
	tmp = argv->next;
	while (tmp && tmp->value[0] == '-' && only_contains(tmp->value, "n"))
	{
		newline = 0;
		tmp = tmp->next;
	}
	while (tmp)
	{
		printf("%s", tmp->value);
		tmp = tmp->next;
		if (tmp)
			printf(" ");
	}
	if (newline)
		printf("\n");
	return (0);
}

int	exec_cd(t_token *argv, t_env *env_list)
{
	char	*path;
	char	*home;

	if (!argv->next || !argv->next->value)
	{
		home = getenv("HOME");
		if (!home)
		{
			write(2, "minishell: cd: HOME not set\n", 28);
			return (1);
		}
		path = home;
	}
	else
		path = argv->next->value;
	if (chdir(path) == -1)
	{
		write(2, "minishell: cd: ", 15);
		perror(path);
		return (1);
	}
	(void)env_list;
	return (0);
}

int	exec_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	else
	{
		perror("minishell: pwd");
		return (1);
	}
}

static t_env	*find_env(t_env *env_list, char *key)
{
	while (env_list)
	{
		if (ft_strcmp(env_list->key, key) == 0)
			return (env_list);
		env_list = env_list->next;
	}
	return (NULL);
}

static void	update_env(t_env *env_list, char *key, char *value)
{
	t_env	*node;

	node = find_env(env_list, key);
	if (node)
	{
		free(node->value);
		node->value = ft_strdup(value);
	}
	else
	{
		node = malloc(sizeof(t_env));
		if (!node)
			return ;
		node->key = ft_strdup(key);
		node->value = ft_strdup(value);
		node->next = env_list->next;
		env_list->next = node;
	}
}

static char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == c)
			return ((char *)s);
		s++;
	}
	if (c == 0)
		return ((char *)s);
	return (NULL);
}

static void	print_export(t_env *env_list)
{
	if (!env_list)
		return ;
	while (env_list)
	{
		printf("declare -x %s=\"%s\"\n", env_list->key, env_list->value);
		env_list = env_list->next;
	}
}

int	exec_export(t_token *argv, t_env *env_list)
{
	t_token	*tmp;
	char	*eq;
	char	*key;

	tmp = argv->next;
	if (!tmp)
		return (print_export(env_list), 0);
	while (tmp)
	{
		eq = ft_strchr(tmp->value, '=');
		if (eq)
		{
			key = ft_strndup(tmp->value, eq - tmp->value);
			update_env(env_list, key, eq + 1);
			free(key);
		}
		tmp = tmp->next;
	}
	return (0);
}

static void	remove_env(t_env **env_list, char *key)
{
	t_env	*prev;
	t_env	*current;

	prev = NULL;
	current = *env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			free(current->key);
			free(current->value);
			free(current);
			break ;
		}
		prev = current;
		current = current->next;
	}
}

int	exec_unset(t_token *argv, t_env *env_list)
{
	t_token	*tmp;

	tmp = argv->next;
	while (tmp)
	{
		remove_env(&env_list, tmp->value);
		tmp = tmp->next;
	}
	return (0);
}

int	exec_env(t_env *env_list)
{
	while (env_list)
	{
		printf("%s=%s\n", env_list->key, env_list->value);
		env_list = env_list->next;
	}
	return (0);
}

static int	ft_atoi(const char *str)
{
	int		i;
	int		sign;
	long	result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return ((int)(result * sign));
}

int	exec_exit(t_token *argv)
{
	int	exit_code;

	printf("exit\n");
	if (argv->next)
	{
		exit_code = ft_atoi(argv->next->value);
		exit(exit_code);
	}
	exit(0);
}
