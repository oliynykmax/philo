# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/17 16:00:00 by maoliiny          #+#    #+#              #
#    Updated: 2025/05/22 17:52:19 by maoliiny         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    := pipex

CC      := cc
CFLAGS  := -Wall -Wextra -Werror

INCLUDES := -Iincl -Ilibft

SRCS    := srcs/pipex.c srcs/pipex_utils.c
OBJS    := $(SRCS:srcs/%.c=obj/%.o)

LIBFT_DIR := libft
LIBFT_A   := $(LIBFT_DIR)/libft.a

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT_A)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) $(LIBFT_A) -o $(NAME)

obj/%.o: srcs/%.c incl/pipex.h | objs
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

objs:
	mkdir -p obj

$(LIBFT_A):
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR)
	@echo "make libft"

clean:
	rm -f $(OBJS)
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) clean
	@echo "clean libft"

fclean: clean
	rm -f $(NAME)
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) fclean
	@echo "fclean libft"

re:
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all
