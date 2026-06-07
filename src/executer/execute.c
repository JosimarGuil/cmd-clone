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

    if (!node->args || !node->args[0])
    {
        shell->last_exit_status = 127;
        return 127;
    }

    if (is_builtin(node->args[0]))
    {
        int ret = execute_builtin(node, shell);
        shell->last_exit_status = ret;
        return ret;
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        shell->last_exit_status = 1;
        return 1;
    }

    if (pid == 0)
        exec_cmd_child(node);

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

    apply_redirections(node->redirs);

    execvp(node->args[0], node->args);

    perror(node->args[0]);

    if (errno == ENOENT)
        exit(127);
    exit(126);
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

        exit(execute_ast(node->left, shell));
    }

    right_pid = fork();
    if (right_pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);

        exit(execute_ast(node->right, shell));
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