#include "../../includes/minishell.h"

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