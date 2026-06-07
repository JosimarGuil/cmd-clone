#include "../../includes/minishell.h"

int builtin_cd(t_shell *shell, char **args)
{
    char *path;
    if (!args[1])
        path = get_env_value(shell->env, "HOME");
    else
        path = args[1];
    if (!path)
    {
        printf("cd: HOME not set\n");
        return 1;
    }
    if (chdir(path) != 0)
    {
        perror("cd");
        return (1);
    }
    return (0);
}