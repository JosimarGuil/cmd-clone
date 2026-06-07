# include "../../includes/minishell.h"

int prepare_heredocs(t_ast_node *node)
{
    t_redir *r;

    if (!node)
        return (0);

    if (node->type == NODE_CMD)
    {
        r = node->redirs;

        while (r)
        {
            if (r->type == TOKEN_HEREDOC)
            {
                r->heredoc_fd = create_heredoc(r->file);

                if (r->heredoc_fd == -1)
                    return (130);
            }

            r = r->next;
        }
    }

    if (prepare_heredocs(node->left))
        return (130);

    if (prepare_heredocs(node->right))
        return (130);

    return (0);
}

static void write_heredoc(int fd, char *limiter)
{
    char *line;

    while (1)
    {
        line = readline("> ");

        if (!line)
            break;

        if (strcmp(line, limiter) == 0)
        {
            free(line);
            break;
        }

        write(fd, line, strlen(line));
        write(fd, "\n", 1);
        free(line);
    }
}

int create_heredoc(char *limiter)
{
    int fd[2];
    pid_t pid;
    int status;

    pipe(fd);

    pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);

        write_heredoc(fd[1], limiter);

        close(fd[1]);

        rl_clear_history();
        exit(0);
    }

    signal(SIGINT, SIG_IGN);

    waitpid(pid, &status, 0);

    signal(SIGINT, sigint_prompt);

    close(fd[1]);

    if (WIFSIGNALED(status) || WEXITSTATUS(status) == 130)
    {
        close(fd[0]);
        return (-1);
    }

    return fd[0];
}