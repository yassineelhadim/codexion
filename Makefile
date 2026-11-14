# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yel-hadi <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/11 by yel-hadi                   #+#    #+#              #
#    Updated: 2026/11/14 14:23:48 by yel-hadi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = main.c \
      cleanup.c \
      coder_utils.c \
      coder.c \
      comparators.c \
      console.c \
      dongle_utils.c \
      dongle.c \
      getters.c \
      heap_utils.c \
      heap.c \
      init_coders.c \
      init_dongles.c \
      init_priority_utils.c \
      init_utils.c \
      init.c \
      monitor.c \
      parser_utils.c \
      parser.c \
      safe_wrappers_utils.c \
      safe_wrappers.c \
      setters.c \
      simulation.c \
      utils.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
