#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig)
{
    printf("\nCaught SIGINT (Ctrl+C)\n");
}

int main(void)
{
    char buffer[100];

    signal(SIGINT, handler);

    printf("minishell$ ");
    ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
    if (bytes == -1)
    {
        perror("read");
    }
    else
    {
        buffer[bytes] = '\0';
        printf("You typed: %s\n", buffer);
    }

    printf("exit\n");
    return 0;
}
