#include "minishell.h"

int	expand_cmd_argv(t_token *argv, t_env *env_list, int *status)
{
	size_t	j;

	while (argv)
	{
		j = 0;
		while (argv->value && argv->value[j] != '\0')
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
			if (argv->value == NULL)
				return (-1);
		}
		argv = argv->next;
	}
	return (0);
}

int	expand_cmd_fd(t_fd *fd, t_env *env_list, int *status)
{
	size_t	j;

	while (fd)
	{
		j = 0;
		while (fd->value && fd->value[j] != '\0')
		{
			if (fd->value[j] == '\'')
				fd->value = trim_quote(fd->value, &j, '\'');
			else if (fd->value[j] == '"')
				fd->value = trim_double_quote(fd->value, &j, env_list, status);
			else if (fd->value[j] == '$')
				fd->value = parse_env_var(fd->value, &j, env_list, status);
			else
				j++;
			if (fd->value == NULL)
				return (-1);
		}
		fd = fd->next;
	}
	return (0);
}

int	expand_cmd(t_cmd *cmd, t_env *env_list, int *status)
{
	if (expand_cmd_argv(cmd->argv, env_list, status) == -1
		|| expand_cmd_fd(cmd->fds, env_list, status) == -1
		|| expand_cmd_fd(cmd->heredoc_delimiter, env_list, status) == -1)
		return (-1);
	return (0);
}

int	expander(t_node *node, t_env *env_list, int *status)
{
	if (!node)
		return (0);
	if (node->type == NODE_CMD)
		return (expand_cmd(node->cmd, env_list, status));
	else
	{
		if (expander(node->lhs, env_list, status) == -1)
			return (-1);
		if (node->type == NODE_PIPE)
			return (expander(node->rhs, env_list, status));
		return (0);
	}
}
