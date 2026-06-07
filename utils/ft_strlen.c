#include "../includes/minishell.h"

int ft_strlen(const char *s)
{
    int len = 0;

    while (s[len])
        len++;

    return len;
}