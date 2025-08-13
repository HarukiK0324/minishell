int pipefd[2];
pipe(pipefd);

pid_t pid = fork();
if (pid == 0) {
    // Child
    close(pipefd[0]); // close unused read end immediately

    // Signal handlers for SIGINT/SIGQUIT
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);

    char *line;
    while ((line = readline("> ")) != NULL) {
        write(pipefd[1], line, strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
    }

    close(pipefd[1]); // close write end normally
    _exit(0);
}
else {
    // Parent
    close(pipefd[1]); // close unused write end

    char buf[1024];
    ssize_t n;
    while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
        // process heredoc data
    }

    close(pipefd[0]); // done reading
    waitpid(pid, NULL, 0);
}
