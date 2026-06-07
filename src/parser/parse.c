# include "../../includes/minishell.h"

static int get_precedence(t_token_type type) 
{
    if (type == TOKEN_AND || type == TOKEN_OR) return 1;
    if (type == TOKEN_PIPE) return 2;
    return 0;
}

int is_syntax_valid(t_token *t)
{
    if (!t)
        return (0);

    // primeiro token não pode ser operador
    if (t->type == TOKEN_PIPE || t->type == TOKEN_AND || t->type == TOKEN_OR)
        return (0);

    while (t)
    {
        // operador no fim é inválido
        if ((t->type == TOKEN_PIPE || t->type == TOKEN_AND || t->type == TOKEN_OR)
            && (!t->next || t->next->type == TOKEN_PIPE
                || t->next->type == TOKEN_AND
                || t->next->type == TOKEN_OR))
            return (0);

        // redireções sem argumento
        if (is_redirection(t->type))
        {
            if (!t->next || t->next->type != TOKEN_WORD)
                return (0);
        }

        t = t->next;
    }
    return (1);
}


static t_ast_node *parse_simple_command(t_token **tokens) 
{
    t_ast_node *node = calloc(1, sizeof(t_ast_node));
    node->type = NODE_CMD;

    while (*tokens) 
    {
        if (get_precedence((*tokens)->type) > 0)
            break;
        if ((*tokens)->type == TOKEN_WORD) 
        {
            node->args = add_to_array(node->args, (*tokens)->value);
            *tokens = (*tokens)->next;
        }
        else if (is_redirection((*tokens)->type)) 
        {
            t_token_type redir_type = (*tokens)->type;
            *tokens = (*tokens)->next;
            if (*tokens && (*tokens)->type == TOKEN_WORD) 
            {
                add_redir_back(&node->redirs, redir_type, (*tokens)->value);
                *tokens = (*tokens)->next;
            } else 
            {
                printf("minishell: syntax error near unexpected token\n");
            }
        }
    }
    return node;
}

t_ast_node *parse_expression(t_token **tokens, int min_prec) 
{
    t_ast_node *left;
    int prec;
    t_token *op;
    t_ast_node *new_node;

    left = parse_simple_command(tokens);
    while (*tokens) {
        prec = get_precedence((*tokens)->type);
        if (prec == 0 || prec < min_prec)
            break;

        op = *tokens;
        *tokens = (*tokens)->next; // Consome o operador (&&, || ou |)

        new_node = calloc(1, sizeof(t_ast_node));
        new_node->type = convert_token_to_node_type(op->type);
        new_node->left = left;

        // O lado direito é processado com a precedência atual
        new_node->right = parse_expression(tokens, prec);

        left = new_node;
    }
    return left;
}

