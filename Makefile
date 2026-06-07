NAME = minishell

CC = cc

CFLAGS = -Wall -Wextra -Werror

FILES = src/main.c src/tokenizer/tokenize.c src/parser/parse.c  utils/ft_strcmp.c\
		src/parser/parse_utils.c src/executer/execute.c auxs/auxs_executor.c\
		src/builtins/builtins.c src/builtins/echo.c src/tokenizer/tokenize_utils.c\
		src/builtins/pwd.c src/builtins/env.c auxs/env_utils.c auxs/expander_env.c\
		utils/ft_strlen.c utils/ft_isalpha.c utils/ft_isalnum.c src/builtins/cd.c\
		src/heredoc/heredoc.c src/signals/signals.c src/builtins/exit.c src/builtins/unset.c\
		utils/ft_itoa.c
		
LIBS = -lreadline

OBJS = $(FILES:.c=.o)

all: $(NAME)

$(NAME): $(OBJS) includes/minishell.h
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LIBS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re