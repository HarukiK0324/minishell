#include "minishell.h"

int	ft_isblank(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

int	ismetachar(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '(' || c == ')' || c == ';'
		|| c == '&' || c == '`' || c == '\\');
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	while (n-- && *s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	if (n == (size_t)-1)
		return (0);
	return (*(const unsigned char *)s1 - *(const unsigned char *)s2);
}

char	*ft_strdup(const char *s)
{
	char	*str;
	size_t	i;

	if (!s)
		return (NULL);
	i = 0;
	str = (char *)malloc(ft_strlen(s) + 1);
	if (!str)
		return (perror("malloc"), NULL);
	while (s[i] != '\0')
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*str;
	size_t	i;

	i = 0;
	str = (char *)malloc(n + 1);
	if (!str)
		return (perror("malloc"), NULL);
	while (i < n && s[i] != '\0')
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}
