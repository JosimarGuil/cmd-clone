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
    int start;
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
        if (input[i] == '"' || input[i] == '\'')
        {
            char quote;
            char *value;
            t_quote_type qtype;
            t_token *tok;

            quote = input[i];

            if (quote == '\'')
                qtype = SINGLE_QUOTE;
            else
                qtype = DOUBLE_QUOTE;

            i++;
            start = i;

            while (input[i] && input[i] != quote)
                i++;

            value = strndup(&input[start], i - start);

            tok = new_token(value, TOKEN_WORD, qtype);

            add_token(&tokens, tok);

            if (input[i] == quote)
                i++;

            continue;
        }

        if (input[i] == '&' && input[i + 1] == '&')
        {
            add_token(&tokens, new_token("&&", TOKEN_AND, NO_QUOTE));
            i += 2;
            continue;
        }
        start = i;
        while (input[i] && input[i] != ' ' && input[i] != '|'
        && input[i] != '>' && input[i] != '<' && input[i] != '"' && input[i] != '\'')
            i++;
        char *word = strndup(&input[start], i - start);
        add_token(&tokens, new_token(word, TOKEN_WORD, NO_QUOTE));
    }
    return tokens;
}