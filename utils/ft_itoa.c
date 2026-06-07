#include "../includes/minishell.h"

char *ft_itoa(int n)
{
    char *str;
    long num;
    int len;

    num = n;
    len = (n <= 0) ? 1 : 0;
    while (num)
    {
        num /= 10;
        len++;
    }
    str = malloc(len + 1);
    if (!str)
        return (NULL);
    str[len] = '\0';
    if (n == 0)
        str[0] = '0';
    else if (n < 0)
        str[0] = '-';
    while (n)
    {
        str[--len] = '0' + abs(n % 10);
        n /= 10;
    }
    return (str);
}