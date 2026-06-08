# include "../../includes/minishell.h"

static int exec_cmd(t_ast_node *node, t_shell *shell);
static void exec_cmd_child(t_ast_node *node);
static int exec_pipe(t_ast_node *node, t_shell *shell);
static int exec_and(t_ast_node *node, t_shell *shell);
static int exec_or(t_ast_node *node, t_shell *shell);


static int get_exit_status(int status)
{
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    return 1;
}

int check_redirections(t_redir *redir)
{
    int fd;

    while (redir)
    {
        if (redir->type == TOKEN_REDIR_IN)
        {
            fd = open(redir->file, O_RDONLY);
            if (fd < 0)
                return (perror(redir->file), 1);
            close(fd);
        }
        else if (redir->type == TOKEN_REDIR_OUT)
        {
            fd = open(redir->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0)
                return (perror(redir->file), 1);
            close(fd);
        }
        else if (redir->type == TOKEN_APPEND)
        {
            fd = open(redir->file, O_CREAT | O_WRONLY | O_APPEND, 0644);
            if (fd < 0)
                return (perror(redir->file), 1);
            close(fd);
        }
        redir = redir->next;
    }
    return (0);
}

int execute_ast(t_ast_node *node, t_shell *shell)
{
    if (!node)
        return (shell->last_exit_status = 1, 1);

    if (node->type == NODE_CMD)
        return exec_cmd(node, shell);
    else if (node->type == NODE_PIPE)
        return exec_pipe(node, shell);
    else if (node->type == NODE_AND)
        return exec_and(node, shell);
    else if (node->type == NODE_OR)
        return exec_or(node, shell);

    return (shell->last_exit_status = 1, 1);
}

static int exec_cmd(t_ast_node *node, t_shell *shell)
{
    pid_t pid;
    int status;

    if (!node->args || !node->args[0] || node->args[0][0] == '\0')
        return (shell->last_exit_status = 0, 0);

    if (check_redirections(node->redirs))
        return (shell->last_exit_status = 1, 1);

    if (is_builtin(node->args[0]))
    {
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stdin = dup(STDIN_FILENO);

        if (apply_redirections(node->redirs))
            return (shell->last_exit_status = 1, 1);

        int ret = execute_builtin(node, shell);

        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);

        close(saved_stdout);
        close(saved_stdin);

        return (shell->last_exit_status = ret, ret);
    }

    pid = fork();
    if (pid < 0)
        return (perror("fork"), shell->last_exit_status = 1, 1);

    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        apply_redirections(node->redirs);

        exec_cmd_child(node);
    }

    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
        shell->last_exit_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        shell->last_exit_status = 128 + WTERMSIG(status);
    else
        shell->last_exit_status = 1;

    return shell->last_exit_status;
}

static void exec_cmd_child(t_ast_node *node)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

    if (apply_redirections(node->redirs))
        exit(1);

    execvp(node->args[0], node->args);

    perror(node->args[0]);

    if (errno == ENOENT)
        exit(127);
    exit(126);
}

int execute_node(t_ast_node *node, t_shell *shell)
{
    if (node->type == NODE_CMD)
        return exec_cmd(node, shell);
    if (node->type == NODE_PIPE)
        return exec_pipe(node, shell);
    if (node->type == NODE_AND)
        return exec_and(node, shell);
    if (node->type == NODE_OR)
        return exec_or(node, shell);
    return 1;
}

static int exec_pipe(t_ast_node *node, t_shell *shell)
{
    int fd[2];
    pid_t left_pid;
    pid_t right_pid;
    int status_left;
    int status_right;

    if (!node->left || !node->right)
        return (shell->last_exit_status = 1, 1);

    if (pipe(fd) == -1)
    {
        perror("pipe");
        return (shell->last_exit_status = 1, 1);
    }

    left_pid = fork();
    if (left_pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        if (apply_redirections(node->left->redirs))
            exit(1);
        exit(execute_node(node->left, shell));
    }

    right_pid = fork();
    if (right_pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        if (apply_redirections(node->right->redirs))
            exit(1);
        exit(execute_node(node->right, shell));
    }
    close(fd[0]);
    close(fd[1]);
    waitpid(left_pid, &status_left, 0);
    waitpid(right_pid, &status_right, 0);
    (void)status_left;
    int st = get_exit_status(status_right);
    shell->last_exit_status = st;
    return st;
}

static int exec_and(t_ast_node *node, t_shell *shell)
{
    int status;

    status = execute_ast(node->left, shell);
    shell->last_exit_status = status;

    if (status == 0)
    {
        status = execute_ast(node->right, shell);
        shell->last_exit_status = status;
    }

    return status;
}

static int exec_or(t_ast_node *node, t_shell *shell)
{
    int status;

    status = execute_ast(node->left, shell);
    shell->last_exit_status = status;

    if (status != 0)
    {
        status = execute_ast(node->right, shell);
        shell->last_exit_status = status;
    }

    return status;
}