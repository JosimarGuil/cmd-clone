#include "../../includes/minishell.h"

int is_builtin(char *cmd)
{
    if (!cmd)
        return (0);

    return (
        strcmp(cmd, "echo") == 0 ||
        strcmp(cmd, "cd") == 0 ||
        strcmp(cmd, "pwd") == 0 ||
        strcmp(cmd, "env") == 0 ||
        strcmp(cmd, "export") == 0 ||
        strcmp(cmd, "unset") == 0 ||
        strcmp(cmd, "exit") == 0
    );
}

int execute_builtin(t_ast_node *node, t_shell *shell)
{
    if (strcmp(node->args[0], "echo") == 0)
        return builtin_echo(node->args);

    else if (strcmp(node->args[0], "export") == 0)
        return builtin_export(shell, node->args);

    else if (strcmp(node->args[0], "pwd") == 0)
        return builtin_pwd();

    else if (strcmp(node->args[0], "env") == 0)
      return builtin_env(shell);

    else if (strcmp(node->args[0], "cd") == 0)
       return builtin_cd(shell, node->args);

    else if (strcmp(node->args[0], "exit") == 0)
        return builtin_exit(node->args);
    else if (strcmp(node->args[0], "unset") == 0)
        return builtin_unset(shell, node->args);

    return (1);
}