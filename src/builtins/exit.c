#include "../../includes/minishell.h"

int is_numeric(char *str)
{
    int i;

    i = 0;

    if (!str || !str[0])
        return (0);

    if (str[i] == '+' || str[i] == '-')
        i++;

    if (!str[i])
        return (0);

    while (str[i])
    {
        if (!isdigit(str[i]))
            return (0);

        i++;
    }

    return (1);
}

int builtin_exit(char **args)
{
    long status;

    printf("exit\n");

    if (!args[1])
        exit(0);

    if (!is_numeric(args[1]))
    {
        printf("minishell: exit: %s: numeric argument required\n",
            args[1]);

        exit(255);
    }

    if (args[2])
    {
        printf("minishell: exit: too many arguments\n");
        return (1);
    }

    status = atol(args[1]);

    exit((unsigned char)status);
}