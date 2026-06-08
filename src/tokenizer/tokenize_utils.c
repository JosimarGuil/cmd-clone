#include "../../includes/minishell.h"


char *read_word(char *input, int *i, t_quote_type *quote)
{
    char buffer[4096];
    int j;
    char current_quote;

    j = 0;
    *quote = NO_QUOTE;

    while (input[*i] && input[*i] != ' ' && input[*i] != '|'
        && input[*i] != '>' && input[*i] != '<')
    {
        if (input[*i] == '\'' || input[*i] == '"')
        {
            current_quote = input[*i];

            if (current_quote == '\'')
                *quote = SINGLE_QUOTE;
            else
                *quote = DOUBLE_QUOTE;

            (*i)++;

            while (input[*i] && input[*i] != current_quote)
                buffer[j++] = input[(*i)++];

            if (input[*i] == current_quote)
                (*i)++;

            continue;
        }

        buffer[j++] = input[(*i)++];
    }

    buffer[j] = '\0';
    return strdup(buffer);
}

char **add_to_array(char **array, char *new_str)
{
    int i;
    int j;
    char **new_array;

    i = 0;
    j = 0;
    // Conta quantos elementos já existem
    while (array && array[i])
        i++;
    
    // Aloca espaço para (elementos atuais + novo + NULL)
    new_array = malloc(sizeof(char *) * (i + 2));
    if (!new_array)
        return NULL;

    // Copia os ponteiros antigos para o novo array
    while (j < i)
    {
        new_array[j] = array[j];
        j++;
    }
        
    new_array[i] = strdup(new_str); // Adiciona a nova palavra
    new_array[i + 1] = NULL;        // Termina com NULL para o execve

    free(array); // Liberta o "contentor" antigo, mas não as strings dentro dele
    return new_array;
}