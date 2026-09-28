NAME	= codexion

CC	= cc
CFLAGS	= -Wall -Wextra -Werror -pthread 
SRC	= cleanup.c coder.c dongle.c dongle_wait.c heap.c \
	  init.c log.c codexion.c monitor.c parser.c sift.c state.c \
	  threads.c time.c utils.c
OBJ	= $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ)
fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: clean