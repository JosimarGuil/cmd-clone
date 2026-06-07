#include "../includes/minishell.h"

static void get_actual_path(char *buff, size_t bufsize);
static char  *shell_readline(void);

int main(int argc, char **argv, char **envp) 
{
    char *line;
    t_token *tokens;
    t_token *tmp_tokens;
    t_shell shell;

    shell.env = copy_env(envp);
    shell.last_exit_status = 0;
    (void)argc;
    (void)argv;
    init_signals();
    while (1)
    {
       line  = shell_readline();
       tokens = lexer(line);
       expand_tokens(tokens, &shell);
       tmp_tokens = tokens;
       t_ast_node *ast = parse_expression(&tmp_tokens, 1);
       if (prepare_heredocs(ast) == 130)
       {
            free_ast(ast);
            free(line);
            continue;
       }
       shell.last_exit_status = execute_ast(ast, &shell);
       free_ast(ast);
       free(line);
    }
    return 0;
}

static void get_actual_path(char *buff, size_t bufsize)
{
    if (getcwd(buff, bufsize) == NULL)
        perror("Erro");
}

static char *shell_readline(void)
{
    char cwd[1024];
    char prompt[1100];
    char *line;

    get_actual_path(cwd, sizeof(cwd));
    sprintf(prompt, "%s$ ", cwd);
    line = readline(prompt);
    if (!line)
        exit(0);
    if (*line)
        add_history(line);
    return line;
}