#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
# include <stdlib.h>

int ft_strlen(char *s)
{
    int i;

	i = 0;
    if(s == NULL)
        return 0;
    while(s[i] != '\0')
        i++;
    return i;
}

int main()
{
	char *line;

	while (1)
	{
		line = readline("minishell$ ");
		if (line == NULL || ft_strlen(line) == 0)
		{
			free(line);
			continue;
		}
		printf("line is '%s'\n", line);
		add_history(line);
		free(line);
	}
	printf("exit\n");
	return 0;
}
