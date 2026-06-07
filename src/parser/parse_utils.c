# include "../../includes/minishell.h"

void add_redir_back(t_redir **list, t_token_type type, char *file)
{
    t_redir *new;
    t_redir *tmp;

    new = malloc(sizeof(t_redir));
    if (!new) return;
    new->type = type;
    new->file = strdup(file);
    new->next = NULL;

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

int convert_token_to_node_type(t_token_type type) {
    if (type == TOKEN_PIPE) return NODE_PIPE;
    if (type == TOKEN_AND) return NODE_AND;
    if (type == TOKEN_OR) return NODE_OR;
    return NODE_CMD; // Por padrão, é um comando simples
}

int is_redirection(t_token_type type) {
    return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT || 
            type == TOKEN_APPEND || type == TOKEN_HEREDOC);
}

void free_ast(t_ast_node *node)
{
    if (!node) return;

    // Limpeza recursiva dos ramos
    free_ast(node->left);
    free_ast(node->right);

    // Limpeza dos dados do comando simples
    if (node->type == NODE_CMD)
    {
        for (int i = 0; node->args && node->args[i]; i++)
            free(node->args[i]);
        free(node->args);
        
        t_redir *tmp;
        while (node->redirs)
        {
            tmp = node->redirs->next;
            free(node->redirs->file);
            free(node->redirs);
            node->redirs = tmp;
        }
    }
    free(node);
}