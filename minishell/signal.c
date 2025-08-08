#include "minishell.h"

void	setup_signal_handlers(void)
{
	struct sigaction	sa_int;

	// Set up SIGINT handler (Ctrl+C)
	sa_int.sa_handler = handle_interactive_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	// Ignore SIGQUIT (Ctrl+\)
	signal(SIGQUIT, SIG_IGN);
}

void	handle_interactive_sigint(int sig)
{
	(void)sig;
	g_status = 2;
	rl_on_new_line();
	write(STDOUT_FILENO, "\n", 1);
#if OS
	rl_replace_line("", 0);
#endif
	rl_redisplay();
}

void	reset_default_signal(void)
{
	// Reset to default behavior
	// Set up SIGINT handler (Ctrl+C)
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	reset_heredoc_signal(void)
{
	// Reset to default behavior
	// Set up SIGINT handler (Ctrl+C)
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
}
