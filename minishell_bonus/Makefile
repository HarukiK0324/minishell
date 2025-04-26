CC = cc
CFLAGS = -Wall -Wextra -Werror -I.

NAME = minishell
SRC_DIR = .
OBJ_DIR = obj

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

$(NAME): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

all: $(NAME)

.PHONY: all clean fclean re