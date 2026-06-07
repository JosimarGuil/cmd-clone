#include "../../includes/minishell.h"

int g_signal = 0;

void sigint_prompt(int sig)
{
    (void)sig;

    write(1, "\n", 1);

    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void init_signals(void)
{
    signal(SIGINT, sigint_prompt);
    signal(SIGQUIT, SIG_IGN);
}

void sigint_heredoc(int sig)
{
    (void)sig;

    g_signal = 1;

    write(1, "\n", 1);
    rl_done = 1;
}