#include "../includes/minishell.h"

int apply_redirections(t_redir *redir)
{
    int fd;

    while (redir)
    {
        if (redir->type == TOKEN_REDIR_OUT)
        {
            fd = open(redir->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0)
                return (perror(redir->file), 1);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redir->type == TOKEN_APPEND)
        {
            fd = open(redir->file, O_CREAT | O_WRONLY | O_APPEND, 0644);
            if (fd < 0)
                return (perror(redir->file), 1);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redir->type == TOKEN_REDIR_IN)
        {
            fd = open(redir->file, O_RDONLY);
            if (fd < 0)
                return (perror(redir->file), 1);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        else if (redir->type == TOKEN_HEREDOC)
        {
            dup2(redir->heredoc_fd, STDIN_FILENO);
            close(redir->heredoc_fd);
        }
        redir = redir->next;
    }
    return (0);
}