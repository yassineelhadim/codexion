NAME = codexion
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread
SRCS = main.c parsing.c parsing_utils.c c_monitor.c \
coder_routine.c create_init.c launch.c mon_help.c \
routine_help.c init_dongles.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c parsing.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re