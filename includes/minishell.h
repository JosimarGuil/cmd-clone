#ifndef MINISHELL_H

# define MINISHELL_H
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <errno.h>
extern int g_signal;

typedef enum e_quote_type
{
    NO_QUOTE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
} t_quote_type;

typedef enum 
{ 
    NODE_CMD, 
    NODE_PIPE, 
    NODE_AND, 
    NODE_OR 
} NodeType;

typedef struct s_shell
{
    char **env;
    int last_exit_status;
} t_shell;

typedef enum e_token_type
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIR_IN,
    TOKEN_REDIR_OUT,
    TOKEN_APPEND,
    TOKEN_HEREDOC,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_GLOB,
}   t_token_type;

typedef struct s_token
{
    char            *value;
    t_token_type    type;
    t_quote_type    quote;
    struct s_token  *next;
}   t_token;

typedef struct s_redir 
{
    t_token_type type;
    char *file;
    int heredoc_fd;
    struct s_redir *next;
} t_redir;

typedef struct s_ast_node 
{
    NodeType       type;
    char              **args;
    t_redir           *redirs;
    struct s_ast_node *left;
    struct s_ast_node *right;
} t_ast_node;


void init_signals(void);
void sigint_prompt(int sig);
void sigint_heredoc(int sig);
//tokenize.c
t_token *lexer(char *input);

//tokenize_utils.c
char **add_to_array(char **array, char *new_str);
char *read_word(char *input, int *i, t_quote_type *quote);
//parse.c
t_ast_node *parse_expression(t_token **tokens, int min_prec);

//parse_utils.c
void add_redir_back(t_redir **list, t_token_type type, char *file);
int convert_token_to_node_type(t_token_type type);
int is_redirection(t_token_type type);
void free_ast(t_ast_node *node);

//execute.c

int execute_ast(t_ast_node *node, t_shell *shell);
void apply_redirections(t_redir *redir);

void print_ast(t_ast_node *node, int level);
int ft_isalpha(int c);
int ft_isalnum(int c);
int ft_strcmp(const char *s1, const char *s2);
int ft_strlen(const char *s);
int is_builtin(char *cmd);
char *ft_itoa(int n);
int execute_builtin(t_ast_node *node, t_shell *shell);
int builtin_echo(char **args);
int builtin_pwd(void);
int builtin_cd(t_shell *shell, char **args);

int create_heredoc(char *limiter);
int prepare_heredocs(t_ast_node *node);

char **copy_env(char **envp);
int find_env_index(char **env, char *key);
int builtin_unset(t_shell *shell, char **args);
void remove_env_var(t_shell *shell, char *key);
int builtin_env(t_shell *shell);
int builtin_export(t_shell *shell, char **args);
int ft_strlen(const char *s);
int ft_strncmp(const char *s1, const char *s2, size_t n);
char *get_env_value(char **env, char *key);
void expand_tokens(t_token *tokens, t_shell *shell);
char *expand_exit_status(t_shell *shell);
int builtin_exit(char **args);



#endif