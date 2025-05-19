#include "minishell.h"

int main(int argc, char *argv[], char *envp[])
{
    char *args[] = {"gr", "sfsdfsdfsdfsdfsdf",NULL}; // Arguments for the command
    if(execve("/usr/bin/grep",args,envp) == -1)
    {
        printf("failed");
    }
}