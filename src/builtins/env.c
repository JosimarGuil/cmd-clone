#include "../../includes/minishell.h"

char **copy_env(char **envp)
{
    int i;
    char **new_env;

    i = 0;

    while (envp[i])
        i++;

    new_env = malloc(sizeof(char *) * (i + 1));

    i = 0;
    while (envp[i])
    {
        new_env[i] = strdup(envp[i]);
        i++;
    }

    new_env[i] = NULL;

    return (new_env);
}

int builtin_env(t_shell *shell)
{
    int i;

    i = 0;

    while (shell->env[i])
    {
        printf("%s\n", shell->env[i]);
        i++;
    }
    return (0);
}