#include "minishell.h"

int only_contains(char *str, char *chars)
{
    int i;
    int j;

    i = 1;
    while(str[i] != '\0')
    {
        j = 0;
        while(chars[j] != '\0')
        {
            if(str[i] == chars[j])
                break;
            j++;
        }
        if(chars[j] == '\0')
            return 0;
        i++;
    }
    return 1;
}

//echo with option -n
void ft_echo(int argc, char *argv[])
{
    int i;
    int newline;
    
    i = 1;
    newline = 1;
    while(1)
    {
        if(argv[i][0] == '-' && only_contains(argv[i], "n"))
        {
            newline = 0;
            i++;
        }
        else
            break;
    }
    while(argv[i] != NULL)
    {
        printf("%s", argv[i]);
        i++;
        if(argv[i] != NULL)
            printf(" ");
    }
    if(newline)
        printf("\n");
}

//cd with only a relative or absolute path
//pwd with no options
//export with no options
//unset with no options
//env with no options or arguments
//exit with no optioncds

int main()
{

}