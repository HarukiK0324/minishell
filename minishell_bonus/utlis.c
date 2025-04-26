#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void ft_pwd(int argc)
{
    char *pwd;

    if(argc != 1)
    {
        fprintf(stderr, "pwd: too many argument\n");
        return;
    }
    pwd = (char *)malloc(1024);
    getcwd(pwd, 1024);
    printf("%s\n",pwd);
}