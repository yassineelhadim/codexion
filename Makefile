NAME	= codexion
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

SRCS	= main.c arguments.c environment_cleanup.c priority_heap.c priority_heap_utils.c dongle_access.c coder_thread.c burnout_monitor.c simulation_time.c interruptible_sleep.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
