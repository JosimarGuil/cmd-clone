#include "../includes/minishell.h"

int env_size(char **env)
{
    int i;

    i = 0;

    while (env[i])
        i++;

    return (i);
}

int is_valid_identifier(char *str)
{
    int i;
    if (!str)
        return (0);
    if (!ft_isalpha(str[0]) && str[0] != '_')
        return (0);
    i = 1;
    while (str[i] && str[i] != '=')
    {
        if (!ft_isalnum(str[i]) && str[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

char *get_env_value(char **env, char *key)
{
    int i;
    int len;

    i = 0;
    len = ft_strlen(key);

    while (env[i])
    {
        if (strncmp(env[i], key, len) == 0
            && env[i][len] == '=')
            return (&env[i][len + 1]);
        i++;
    }
    return (NULL);
}

void free_env(char **env)
{
    int i;

    i = 0;

    while (env[i])
    {
        free(env[i]);
        i++;
    }
    free(env);
}

void add_env_var(t_shell *shell, char *new_var)
{
    char **new_env;
    int size;
    int i;

    size = env_size(shell->env);

    new_env = malloc(sizeof(char *) * (size + 2));

    i = 0;
    while (shell->env[i])
    {
        new_env[i] = strdup(shell->env[i]);
        i++;
    }

    new_env[i] = strdup(new_var);
    new_env[i + 1] = NULL;

    free_env(shell->env);

    shell->env = new_env;
}

char *extract_key(char *var)
{
    int len;
    char *key;

    len = 0;
    while (var[len] && var[len] != '=')
        len++;

    key = malloc(len + 1);
    if (!key)
        return (NULL);

    strncpy(key, var, len);
    key[len] = '\0';

    return (key);
}


int builtin_export(t_shell *shell, char **args)
{
    int i;

    if (!args[1])
    {
        builtin_env(shell);
        return (0);
    }
    i = 1;
    while (args[i])
    {
        if (!is_valid_identifier(args[i]))
        {
            printf("minishell: export: `%s': not a valid identifier\n", args[i]);
        }
        else if (!strchr(args[i], '='))
        {
            i++;
            continue;
        }
        else
        {
            char *key;
            int index;
            key = extract_key(args[i]);
            index = find_env_index(shell->env, key);
            if (index != -1)
            {
                free(shell->env[index]);
                shell->env[index] = strdup(args[i]);
            }
            else
            {
                add_env_var(shell, args[i]);
            }
            free(key);
        }
        i++;
    }
    return (0);
}

int find_env_index(char **env, char *key)
{
    int i;
    int len;

    i = 0;
    len = ft_strlen(key);
    while (env[i])
    {
        if (ft_strncmp(env[i], key, len) == 0 && env[i][len] == '=')
            return (i);
        i++;
    }
    return (-1);
}

void remove_env_var(t_shell *shell, char *key)
{
    int index;
    int i;
    index = find_env_index(shell->env, key);
    if (index == -1)
        return;
    free(shell->env[index]);
    i = index;
    while (shell->env[i])
    {
        shell->env[i] = shell->env[i + 1];
        i++;
    }
}