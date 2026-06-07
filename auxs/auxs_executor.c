#include "../includes/minishell.h"

void apply_redirections(t_redir *redir)
{
    int fd;

    while (redir)
    {
        if (redir->type == TOKEN_REDIR_OUT)
        {
            fd = open(redir->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (redir->type == TOKEN_APPEND)
        {
            fd = open(redir->file, O_CREAT | O_WRONLY | O_APPEND, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (redir->type == TOKEN_HEREDOC)
        {
            dup2(redir->heredoc_fd, STDIN_FILENO);
            close(redir->heredoc_fd);
        }
        else if (redir->type == TOKEN_REDIR_IN)
        {
            fd = open(redir->file, O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        redir = redir->next;
    }
}