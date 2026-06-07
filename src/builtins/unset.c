#include "../../includes/minishell.h"

int builtin_unset(t_shell *shell, char **args)
{
    int i;

    i = 1;

    while (args[i])
    {
        remove_env_var(shell, args[i]);
        i++;
    }

    return (0);
}