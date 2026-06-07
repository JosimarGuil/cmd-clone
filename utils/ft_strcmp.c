#include "../includes/minishell.h"

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
    while (n-- > 0)
    {
        if (*s1 != *s2)
            return (unsigned char)(*s1) - (unsigned char)(*s2);
        if (*s1 == '\0')
            break;
        s1++;
        s2++;
    }
    return 0;
}