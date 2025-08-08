#include "minishell.h"

static t_env	*find_env(t_env *env_list, char *key);

static int	is_valid_identifier(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	if (!((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z')
			|| *str == '_'))
		return (0);
	i = 1;
	while (str[i])
	{
		if (!((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A'
					&& str[i] <= 'Z') || (str[i] >= '0' && str[i] <= '9')
				|| str[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static int	validate_digit_sequence(char *str, int start)
{
	int	i;

	i = start;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	validate_negative_long(char *str, int i)
{
	int		len;
	char	*long_min_str = "9223372036854775808";

	if (!str[i])
		return (0);
	len = ft_strlen(&str[i]);
	if (len > 19)
		return (0);
	if (len == 19 && ft_strcmp(&str[i], long_min_str) > 0)
		return (0);
	return (validate_digit_sequence(str, i));
}

static int	validate_positive_long(char *str, int i)
{
	int		len;
	char	*long_max_str = "9223372036854775807";

	if (!str[i])
		return (0);
	len = ft_strlen(&str[i]);
	if (len > 19)
		return (0);
	if (len == 19 && ft_strcmp(&str[i], long_max_str) > 0)
		return (0);
	return (validate_digit_sequence(str, i));
}

static int	is_valid_long(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '+')
		i++;
	if (str[i] == '-')
		return (validate_negative_long(str, i + 1));
	else
		return (validate_positive_long(str, i));
}

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
	while (tmp && tmp->value[0] == '-' && tmp->value[1] != '\0'
		&& only_contains(tmp->value, "n"))
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

static char	*find_pwd_value(t_env *env_list)
{
	t_env	*pwd_env;

	pwd_env = env_list;
	while (pwd_env && ft_strcmp(pwd_env->key, "PWD") != 0)
		pwd_env = pwd_env->next;
	if (!pwd_env || !pwd_env->value)
		return (NULL);
	return (pwd_env->value);
}

static void	copy_parent_path(char *parent, char *pwd, int len)
{
	int	j;

	if (len == 0)
	{
		parent[0] = '/';
		parent[1] = '\0';
	}
	else
	{
		j = -1;
		while (++j < len)
			parent[j] = pwd[j];
		parent[j] = '\0';
	}
}

static char	*get_parent_from_pwd(t_env *env_list)
{
	char	*pwd;
	char	*parent;
	int		i;

	pwd = find_pwd_value(env_list);
	if (!pwd)
		return (NULL);
	parent = malloc(ft_strlen(pwd) + 1);
	if (!parent)
		return (NULL);
	i = ft_strlen(pwd) - 1;
	while (i > 0 && pwd[i] == '/')
		i--;
	while (i > 0 && pwd[i] != '/')
		i--;
	copy_parent_path(parent, pwd, i);
	return (parent);
}

static char	*get_cd_path(t_token *argv, t_env *env_list)
{
	t_env	*home_env;

	if (!argv->next || !argv->next->value)
	{
		home_env = find_env(env_list, "HOME");
		if (!home_env || !home_env->value)
		{
			write(2, "minishell: cd: HOME not set\n", 28);
			return (NULL);
		}
		return (home_env->value);
	}
	return (argv->next->value);
}

static void	update_pwd(t_env *env_list, char *new_pwd)
{
	t_env	*pwd_env;
	t_env	*oldpwd_env;
	char	*old_pwd;

	pwd_env = env_list;
	while (pwd_env && ft_strcmp(pwd_env->key, "PWD") != 0)
		pwd_env = pwd_env->next;
	oldpwd_env = env_list;
	while (oldpwd_env && ft_strcmp(oldpwd_env->key, "OLDPWD") != 0)
		oldpwd_env = oldpwd_env->next;
	if (pwd_env && pwd_env->value)
	{
		old_pwd = pwd_env->value;
		pwd_env->value = ft_strdup(new_pwd);
		if (oldpwd_env)
		{
			free(oldpwd_env->value);
			oldpwd_env->value = old_pwd;
		}
		else
			free(old_pwd);
	}
}

static int	handle_cd_parent_dir(t_env *env_list)
{
	char	*parent_path;

	parent_path = get_parent_from_pwd(env_list);
	if (!parent_path || chdir(parent_path) == -1)
	{
		write(2, "minishell: cd: ", 15);
		perror("..");
		if (parent_path)
			free(parent_path);
		return (1);
	}
	update_pwd(env_list, parent_path);
	free(parent_path);
	return (0);
}

static int	handle_cd_regular(char *path, t_env *env_list)
{
	char	*new_pwd;

	if (chdir(path) == -1)
	{
		write(2, "minishell: cd: ", 15);
		perror(path);
		return (1);
	}
	new_pwd = getcwd(NULL, 0);
	if (new_pwd)
	{
		update_pwd(env_list, new_pwd);
		free(new_pwd);
	}
	return (0);
}

static int	handle_cd_oldpwd(t_env *env_list)
{
	t_env	*oldpwd_env;
	char	*oldpwd;
	int		result;

	oldpwd_env = env_list;
	while (oldpwd_env && ft_strcmp(oldpwd_env->key, "OLDPWD") != 0)
		oldpwd_env = oldpwd_env->next;
	if (!oldpwd_env || !oldpwd_env->value)
	{
		write(2, "minishell: cd: OLDPWD not set\n", 31);
		return (1);
	}
	oldpwd = ft_strdup(oldpwd_env->value);
	result = handle_cd_regular(oldpwd, env_list);
	if (result == 0)
		printf("%s\n", oldpwd);
	free(oldpwd);
	return (result);
}

int	exec_cd(t_token *argv, t_env *env_list)
{
	char	*path;

	if (argv->next && argv->next->next)
	{
		write(2, "minishell: cd: too many arguments\n", 35);
		return (1);
	}
	path = get_cd_path(argv, env_list);
	if (!path)
		return (1);
	if (ft_strcmp(path, "..") == 0)
		return (handle_cd_parent_dir(env_list));
	if (ft_strcmp(path, "-") == 0)
		return (handle_cd_oldpwd(env_list));
	return (handle_cd_regular(path, env_list));
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

static void	update_existing_env(t_env *node, char *value)
{
	free(node->value);
	if (value)
		node->value = ft_strdup(value);
	else
		node->value = NULL;
}

static void	add_new_env(t_env *env_list, char *key, char *value)
{
	t_env	*node;
	t_env	*tail;

	node = malloc(sizeof(t_env));
	if (!node)
		return ;
	node->key = ft_strdup(key);
	if (value)
		node->value = ft_strdup(value);
	else
		node->value = NULL;
	node->next = NULL;
	tail = env_list;
	while (tail->next)
		tail = tail->next;
	tail->next = node;
}

static void	update_env(t_env *env_list, char *key, char *value)
{
	t_env	*node;

	node = find_env(env_list, key);
	if (node)
		update_existing_env(node, value);
	else
		add_new_env(env_list, key, value);
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

static int	count_env(t_env *env_list)
{
	int	count;

	count = 0;
	while (env_list)
	{
		count++;
		env_list = env_list->next;
	}
	return (count);
}

static void	swap_env(t_env **a, t_env **b)
{
	char	*tmp_key;
	char	*tmp_value;

	tmp_key = (*a)->key;
	tmp_value = (*a)->value;
	(*a)->key = (*b)->key;
	(*a)->value = (*b)->value;
	(*b)->key = tmp_key;
	(*b)->value = tmp_value;
}

static void	sort_env_array(t_env **arr, int n)
{
	int	i;
	int	j;

	i = 0;
	while (i < n - 1)
	{
		j = 0;
		while (j < n - i - 1)
		{
			if (ft_strcmp(arr[j]->key, arr[j + 1]->key) > 0)
				swap_env(&arr[j], &arr[j + 1]);
			j++;
		}
		i++;
	}
}

static t_env	**create_env_array(t_env *env_list, int count)
{
	t_env	**arr;
	t_env	*tmp;
	int		i;

	arr = malloc(sizeof(t_env *) * count);
	if (!arr)
		return (NULL);
	tmp = env_list;
	i = 0;
	while (tmp)
	{
		arr[i++] = tmp;
		tmp = tmp->next;
	}
	return (arr);
}

static void	print_sorted_env(t_env **arr, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (arr[i]->value)
			printf("declare -x %s=\"%s\"\n", arr[i]->key, arr[i]->value);
		else
			printf("declare -x %s\n", arr[i]->key);
		i++;
	}
}

static void	print_export(t_env *env_list)
{
	t_env	**arr;
	int		count;

	if (!env_list)
		return ;
	count = count_env(env_list);
	arr = create_env_array(env_list, count);
	if (!arr)
		return ;
	sort_env_array(arr, count);
	print_sorted_env(arr, count);
	free(arr);
}

static void	print_export_error(char *value)
{
	write(2, "minishell: export: `", 20);
	write(2, value, ft_strlen(value));
	write(2, "': not a valid identifier\n", 27);
}

static int	handle_export_with_equals(t_token *tmp, t_env *env_list)
{
	char	*eq;
	char	*key;

	eq = ft_strchr(tmp->value, '=');
	key = ft_strndup(tmp->value, eq - tmp->value);
	if (!is_valid_identifier(key))
	{
		print_export_error(tmp->value);
		free(key);
		return (1);
	}
	update_env(env_list, key, eq + 1);
	free(key);
	return (0);
}

static int	handle_export_without_equals(t_token *tmp, t_env *env_list)
{
	if (!is_valid_identifier(tmp->value))
	{
		print_export_error(tmp->value);
		return (1);
	}
	update_env(env_list, tmp->value, NULL);
	return (0);
}

int	exec_export(t_token *argv, t_env *env_list)
{
	t_token	*tmp;
	int		status;

	tmp = argv->next;
	if (!tmp)
		return (print_export(env_list), 0);
	status = 0;
	while (tmp)
	{
		if (ft_strchr(tmp->value, '='))
			status |= handle_export_with_equals(tmp, env_list);
		else
			status |= handle_export_without_equals(tmp, env_list);
		tmp = tmp->next;
	}
	return (status);
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
			else
				*env_list = current->next;
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
		if (env_list->value)
			printf("%s=%s\n", env_list->key, env_list->value);
		env_list = env_list->next;
	}
	return (0);
}

static long	ft_atol(const char *str)
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
	return (result * sign);
}

int	exec_exit(t_token *argv)
{
	long	exit_code;

	printf("exit\n");
	if (argv->next)
	{
		if (!is_valid_long(argv->next->value))
		{
			write(2, "minishell: exit: ", 17);
			write(2, argv->next->value, ft_strlen(argv->next->value));
			write(2, ": numeric argument required\n", 29);
			exit(2);
		}
		if (argv->next->next)
		{
			write(2, "minishell: exit: too many arguments\n", 37);
			return (1);
		}
		exit_code = ft_atol(argv->next->value);
		exit((int)(exit_code & 0xFF));
	}
	exit(0);
}
