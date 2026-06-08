#include "../../includes/minishell.h"

static t_token *new_token(char *value, t_token_type type, t_quote_type quote)
{
    t_token *token;

    token = malloc(sizeof(t_token));
    if (!token)
        return NULL;
    token->value = value;
    token->type = type;
    token->quote = quote;
    token->next = NULL;
    return token;
}

static void add_token(t_token **list, t_token *new)
{
    t_token *tmp;

    if (!*list)
    {
        *list = new;
        return;
    }
    tmp = *list;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new;
}

t_token *lexer(char *input)
{
    int i = 0;
    t_token *tokens = NULL;

    while (input[i])
    {
        if (input[i] == ' ')
        {
            i++;
            continue;
        }

        if (input[i] == '|' && input[i + 1] == '|')
        {
            add_token(&tokens, new_token("||", TOKEN_OR, NO_QUOTE));
            i += 2;
            continue;
        }
        else if (input[i] == '|')
        {
            add_token(&tokens, new_token("|", TOKEN_PIPE, NO_QUOTE));
            i++;
            continue;
        }

        if (input[i] == '>' && input[i + 1] == '>')
        {
            add_token(&tokens, new_token(">>", TOKEN_APPEND, NO_QUOTE));
            i += 2;
            continue;
        }
        else if (input[i] == '>')
        {
            add_token(&tokens, new_token(">", TOKEN_REDIR_OUT, NO_QUOTE));
            i++;
            continue;
        }
        if (input[i] == '<' && input[i + 1] == '<')
        {
            add_token(&tokens, new_token("<<", TOKEN_HEREDOC, NO_QUOTE));
            i += 2;
            continue;
        }
        else if (input[i] == '<')
        {
            add_token(&tokens, new_token("<", TOKEN_REDIR_IN, NO_QUOTE));
            i++;
            continue;
        }

        if (input[i] == '&' && input[i + 1] == '&')
        {
            add_token(&tokens, new_token("&&", TOKEN_AND, NO_QUOTE));
            i += 2;
            continue;
        }
        while (input[i] == ' ')
            i++;
        char *word;
        t_quote_type q;
        q = NO_QUOTE;
        word = read_word(input, &i, &q);
        if (word)
            add_token(&tokens, new_token(word, TOKEN_WORD, q));
        continue;
    }
    return tokens;
}