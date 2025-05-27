# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/17 16:00:00 by maoliiny          #+#    #+#              #
#    Updated: 2025/05/27 15:36:44 by maoliiny         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    := philo

CC      := cc
CFLAGS  := -Wall -Wextra -Werror

INCLUDES := -Iincl

SRCS    := srcs/philo.c srcs/philo_utils.c
OBJS    := $(SRCS:srcs/%.c=obj/%.o)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS) incl/philo.h
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

obj/%.o: srcs/%.c incl/philo.h | obj
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj:
	@mkdir -p obj

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re:
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

