#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define SIGHUP  1   /* Hang up the process */ 
#define SIGINT  2   /* Interrupt the process */ 
#define SIGQUIT 3   /* Quit the process */ 
#define SIGILL  4   /* Illegal instruction. */ 
#define SIGTRAP 5   /* Trace trap. */ 
#define SIGABRT 6   /* Abort. */

void ft_pwd(int argc)
{
    char *pwd;

    if(argc != 1)
    {
        fprintf(stderr, "pwd: wrong argument\n");
        return;
    }
    pwd = (char *)malloc(1024);
    getcwd(pwd, 1024);
    printf("%s\n",pwd);
}
  
int main() 
{ 
    int i = 1;

    i /= 0;
    while (1) 
    { 
        printf("hello world\n"); 
        sleep(1); 
    } 
    return 0; 
} 