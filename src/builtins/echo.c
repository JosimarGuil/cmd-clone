#include "../../includes/minishell.h"

int builtin_echo(char **args)
{
    int i;
    int newline;

    i = 1;
    newline = 1;

    if (args[1] && ft_strcmp(args[1], "-n") == 0)
    {
        newline = 0;
        i++;
    }

    while (args[i])
    {
        printf("%s", args[i]);

        if (args[i + 1])
            printf(" ");

        i++;
    }

    if (newline)
        printf("\n");

    return (0);
}