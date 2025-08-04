CC = cc
# CFLAGS = -Wall -Wextra -Werror -I.
LDFLAGS = -lreadline
NAME = minishell

SRC_DIR = .
OBJ_DIR = obj

SRC = builtin.c executor.c expander.c ft_split.c main.c parser.c tokenizer.c heredoc.c
OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
