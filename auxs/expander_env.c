#include "../includes/minishell.h"

static void append_str(char *result, char *src, int *j)
{
    int k;

    k = 0;

    while (src[k])
    {
        result[*j] = src[k];
        (*j)++;
        k++;
    }
}

static char *expand_string(char *str, t_shell *shell)
{
    char result[4096];
    char var_name[256];
    char *value;
    int i = 0;
    int j = 0;
    int k;

    while (str[i])
    {
        if (str[i] == '$')
        {
            // CASE 1: $?
            if (str[i + 1] == '?')
            {
                value = ft_itoa(shell->last_exit_status);
                append_str(result, value, &j);
                free(value);
                i += 2;
                continue;
            }

            // CASE 2: $$ (PID do shell)
            if (str[i + 1] == '$')
            {
                value = ft_itoa(getpid());
                append_str(result, value, &j);
                free(value);
                i += 2;
                continue;
            }

            // CASE 3: $VAR (válido)
            if (ft_isalpha(str[i + 1]) || str[i + 1] == '_')
            {
                i++;
                k = 0;

                while (ft_isalnum(str[i]) || str[i] == '_')
                    var_name[k++] = str[i++];

                var_name[k] = '\0';

                value = get_env_value(shell->env, var_name);
                if (value)
                    append_str(result, value, &j);

                continue;
            }

            // CASE 4: $ seguido de nada válido (ex: $abc, $1, $-)
            // REGRA: bash -> deixa o '$' literal
            result[j++] = '$';
            i++;
            continue;
        }

        result[j++] = str[i++];
    }

    result[j] = '\0';
    return strdup(result);
}

char    *expand_exit_status(t_shell *shell)
{
    return ft_itoa(shell->last_exit_status);
}

void expand_tokens(t_token *tokens, t_shell *shell)
{
    char *expanded;

    while (tokens)
    {
        if (tokens->quote == SINGLE_QUOTE)
        {
            tokens = tokens->next;
            continue;
        }

        if (tokens->type == TOKEN_WORD)
        {
            expanded = expand_string(tokens->value, shell);
            free(tokens->value);
            tokens->value = expanded;
        }
        tokens = tokens->next;
    }
}