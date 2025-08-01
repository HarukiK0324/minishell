#include "minishell.h"

int	is_char(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

int	is_numchar(char c)
{
	return (c >= '0' && c <= '9') || is_char(c);
}

char	*to_str(size_t n)
{
	char	*str;
	size_t		len;
	size_t		temp;

	len = 0;
	temp = n;
	if (n == 0)
		return (ft_strdup("0"));
	while (temp > 0)
	{
		temp /= 10;
		len++;
	}
	str = (char *)malloc(len + 1);
	if (!str)
		return (perror("malloc"), NULL);
	str[len] = '\0';
	while (n > 0)
	{
		str[--len] = (n % 10) + '0';
		n /= 10;
	}
	return (str);
}

char	*str_trim(char *str, size_t *j, size_t i)
{
	char	*new_str;
	size_t		k;

	if (!str || *j < 0 || i < 0 || (*j + i >= ft_strlen(str)))
		return (perror("str_trim failed"), NULL);
	new_str = (char *)malloc(ft_strlen(str) - i);
	if (!new_str)
		return (perror("malloc"), NULL);
	k = 0;
	while (k < *j)
	{
		new_str[k] = str[k];
		k++;
	}
	while (str[k + 1 + i] != '\0')
	{
		new_str[k] = str[k + 1 + i];
		k++;
	}
	new_str[k] = '\0';
	free(str);
	return (new_str);
}

char	*str_insert(char *str, size_t *j, char *value)
{
	char	*new_str;
	size_t	k;
	size_t	i;

	if (!str || !value)
		return (perror("str_insert failed"), NULL);
	new_str = (char *)malloc(ft_strlen(str) + ft_strlen(value) + 1);
	if (!new_str)
		return (perror("malloc"), NULL);
	i = -1;
	while (++i < *j)
		new_str[i] = str[i];
	k = -1;
	while (value[++k] != '\0')
		new_str[i + k] = value[k];
	while (str[i] != '\0')
	{
		new_str[i + k] = str[i];
		i++;
	}
	new_str[i + k] = '\0';
	(*j) += k;
	free(str);
	return (new_str);
}

char	*replace_env_var(char *str, size_t *j, size_t i, char *env_var,
		t_env *env_list)
{
	char	*value;

	value = NULL;
	while (env_list)
	{
		if (ft_strcmp(env_var, env_list->key) == 0)
		{
			value = env_list->value;
			break ;
		}
		env_list = env_list->next;
	}
	str = str_trim(str, j, i);
	if (value && ft_strlen(value))
		str = str_insert(str, j, value);
	return (str);
}

char	*replace_status(char *str, size_t *j, int *status)
{
	char	*status_str;

	status_str = to_str(*status);
	if (!status_str)
		return (perror("malloc"), NULL);
	str = str_trim(str, j, 1);
	str = str_insert(str, j, status_str);
	free(status_str);
	return (str);
}

char	*parse_env_var(char *str, size_t *j, t_env *env_list, int *status)
{
	size_t		i;
	size_t		k;
	char	*env_var;

	if (str[*j + 1] == '?')
		return (replace_status(str, j, status));
	i = 0;
	while (is_numchar(str[*j + 1 + i]))
		i++;
	if (i > 0 && is_char(str[*j + 1]))
	{
		env_var = (char *)malloc(i + 1);
		if (!env_var)
			return (perror("malloc"), NULL);
		k = -1;
		while (++k < i)
			env_var[k] = str[*j + 1 + k];
		env_var[k] = '\0';
		str = replace_env_var(str, j, i, env_var, env_list);
	}
	else
		(*j)++;
	return (str);
}

char	*trim_quote(char *str, size_t *j, char c)
{
	size_t		i;
	char	*new_str;

	i = 0;
	new_str = (char *)malloc(ft_strlen(str) - 1);
	while (i < *j)
	{
		new_str[i] = str[i];
		i++;
	}
	while (str[i + 1] != c)
	{
		new_str[i] = str[i + 1];
		(*j)++;
		i++;
	}
	while (str[i + 2] != '\0')
	{
		new_str[i] = str[i + 2];
		i++;
	}
	new_str[i] = '\0';
	free(str);
	return (new_str);
}

char	*trim_double_quote(char *str, size_t *j, t_env *env_list, int *status)
{
	size_t		i;

	i = *j + 1;
	while (str[i] != '"')
	{
		if (str[i] == '$')
			str = parse_env_var(str, &i, env_list, status);
		else
			i++;
	}
	return (trim_quote(str, j, '"'));
}

void	expand_cmd(t_cmd *cmd, t_env *env_list, int *status)
{
	size_t		j;
	t_token	*argv;

	argv = cmd->argv;
	while (cmd && argv)
	{
		j = 0;
		while (argv->value[j] != '\0')
		{
			if (argv->value[j] == '\'')
				argv->value = trim_quote(argv->value, &j, '\'');
			else if (argv->value[j] == '"')
				argv->value = trim_double_quote(argv->value, &j, env_list,
						status);
			else if (argv->value[j] == '$')
				argv->value = parse_env_var(argv->value, &j, env_list, status);
			else
				j++;
		}
		argv = argv->next;
	}
}

void	expander(t_node *node, t_env *env_list, int *status)
{
	if (!node)
		return ;
	if (node->type == NODE_CMD)
		expand_cmd(node->cmd, env_list, status);
	else
	{
		expander(node->lhs, env_list, status);
		if (node->type == NODE_PIPE)
			expander(node->rhs, env_list, status);
		
	}
}
