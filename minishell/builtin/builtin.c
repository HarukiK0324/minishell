#include "minishell.h"

int	is_valid_identifier(char *str)
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

int	validate_digit_sequence(char *str, int start)
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

int	validate_negative_long(char *str, int i)
{
	int		len;
	char	*long_min_str;

	long_min_str = "9223372036854775808";
	if (!str[i])
		return (0);
	len = ft_strlen(&str[i]);
	if (len > 19)
		return (0);
	if (len == 19 && ft_strcmp(&str[i], long_min_str) > 0)
		return (0);
	return (validate_digit_sequence(str, i));
}

int	validate_positive_long(char *str, int i)
{
	int		len;
	char	*long_max_str;

	long_max_str = "9223372036854775807";
	if (!str[i])
		return (0);
	len = ft_strlen(&str[i]);
	if (len > 19)
		return (0);
	if (len == 19 && ft_strcmp(&str[i], long_max_str) > 0)
		return (0);
	return (validate_digit_sequence(str, i));
}

int	is_valid_long(char *str)
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
